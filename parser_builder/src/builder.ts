import "./clua/pattern";

import "./config/pattern";
import "./config/nodes";

import { BaseEmitter } from "./emitter";
import { ClassDescription } from "./clua/descriptions";

function emit_parser_cpp()
{
    // Compose parser using parser-emitter helpers
    class ParserEmitter {
        emitter: BaseEmitter;

        constructor(e: BaseEmitter) { this.emitter = e; }

        emit_nodes_list() {
            const nodes = [
                { name: "Identifier", fields: [{ name: "token", type: "CLua::TokenGeneric" }] },
                { name: "IdentifierPathNode", fields: [{ name: "head", type: "CLua::NodeHandle" }, { name: "is_global", type: "Common::uint8" }, { name: "next_segment", type: "CLua::NodeHandle" }] },
                { name: "GroupExpression", fields: [{ name: "expression", type: "CLua::NodeHandle" }] },
                { name: "CharLiteral", fields: [{ name: "value", type: "Common::uint8" }] },
                { name: "StringLiteral", fields: [{ name: "token", type: "CLua::TokenGeneric" }] },
                { name: "NumberNode", fields: [{ name: "value", type: "Common::f64" }] },
                { name: "IntegerLiteral", fields: [{ name: "value", type: "Common::uint64" }] }
            ];

            this.emitter.emit_node_field_list(nodes);
        }

        emit_consume_symbol() {
            this.emitter.emit_section_header("consume_symbol");
            this.emitter.emit(`bool consume_symbol(ParserContext& parser_context, CLua::SymbolKind expected_symbol)`);
            this.emitter.emit_scope(() => {
                this.emitter.see_current_token("current_token");
                this.emitter.emit(`auto can_be_consumed = current_token.token_type == TokenType::Symbol && parser_context.get_current_symbol() == expected_symbol;`);
                this.emitter.emit(`if (!can_be_consumed) [[unlikely]]`);
                this.emitter.emit_scope(() => { this.emitter.emit(`return false;`); });
                this.emitter.get_next_token();
                this.emitter.emit(`return true;`);
            });
        }

        emit_expect_identifier() {
            this.emitter.emit_section_header("expect_identifier");
            this.emitter.emit(`NodeHandle expect_identifier(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => {
                this.emitter.see_current_token("current_token");
                // guard: is identifier
                this.emitter.emit_guard_clause(this.emitter.is_identifier(), () => {
                    this.emitter.emit(`auto error_node = ${this.emitter.create_node("UnexpectedTokenError", "current_token")};`);
                    this.emitter.emit(`ParserError error = ParserError(current_token);`);
                    this.emitter.emit(`return parser_context.emit_error(error);`);
                });

                this.emitter.get_next_token();
                this.emitter.emit_create_node_assign("identifier_node", "Identifier", "current_token.as<CLua::IdentifierToken>()");
                this.emitter.emit_return("identifier_node");
            });
        }

        emit_expect_group_expression() {
            this.emitter.emit_section_header("expect_group_expression");
            this.emitter.emit(`NodeHandle expect_group_expression(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => {
                // if not LParen -> NoPatternNode
                this.emitter.emit_guard_clause(this.emitter.is_symbol("SymbolKind::LParen"), () => {
                    this.emitter.emit(`return NoPatternNode;`);
                });

                this.emitter.get_next_token();
                this.emitter.emit(`auto expression_node = Expression::expect_expression(parser_context);`);
                this.emitter.emit(`auto return_node = InvalidNode;`);

                this.emitter.emit(`if (expression_node.node_tag == NodeHandleTag::Valid) [[likely]] {`);
                this.emitter.step_indent();
                this.emitter.see_current_token("next_token");
                this.emitter.emit(`if (parser_context.is_symbol(SymbolKind::RParen))`);
                this.emitter.emit_scope(() => {
                    this.emitter.get_next_token();
                    this.emitter.emit(`return parser_context.create_node<GroupExpression>(expression_node);`);
                });
                this.emitter.emit(`else`);
                this.emitter.emit_scope(() => {
                    this.emitter.emit(`ParserError parser_error = ParserError(current_token,next_token);`);
                    this.emitter.emit(`TokenSpan fake_span = TokenSpan(next_token,next_token);`);
                    this.emitter.emit(`parser_error.node_handle = parser_context.create_node<UnclosedBlockError>(fake_span);`);
                    this.emitter.emit(`return parser_context.emit_error(parser_error);`);
                });
                this.emitter.step_dedent();

                this.emitter.emit(`} else if(expression_node.node_tag == NodeHandleTag::Error) {`);
                this.emitter.step_indent();
                this.emitter.emit(`return expression_node;`);
                this.emitter.step_dedent();
                this.emitter.emit(`} else {`);
                this.emitter.step_indent();
                this.emitter.emit(`PAssert(false, "unexpected node tag being returned, this function either returns Valid/Error node tag handle");`);
                this.emitter.emit(`return return_node;`);
                this.emitter.step_dedent();
                this.emitter.emit(`};`);
            });
        }

        emit_expect_scoped_identifier() {
            this.emitter.emit_section_header("expect_scoped_identifier");
            this.emitter.emit(`NodeHandle expect_scoped_identifier(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => {
                this.emitter.emit(`auto optional_global_scope = consume_symbol(parser_context,SymbolKind::DoubleColon);`);
                this.emitter.emit(`auto identifier = expect_identifier(parser_context);`);
                this.emitter.emit(`if (identifier.node_tag != NodeHandleTag::Valid) { return identifier; }`);

                this.emitter.emit(`auto scoped_identifier_head = parser_context.create_node<IdentifierPathNode>(identifier, optional_global_scope);`);
                this.emitter.emit(`auto last_path_node = scoped_identifier_head;`);

                this.emitter.emit(`while (!parser_context.has_reached_end())`);
                this.emitter.emit_scope(() => {
                    this.emitter.emit(`auto scope_enabled = consume_symbol(parser_context, SymbolKind::DoubleColon);`);
                    this.emitter.emit(`if (!scope_enabled) { break; }`);
                    this.emitter.emit(`auto next_identifier = expect_identifier(parser_context);`);
                    this.emitter.emit(`if (next_identifier.node_tag != NodeHandleTag::Valid) { return next_identifier; }`);
                    this.emitter.emit(`auto scoped_identifier = parser_context.create_node<IdentifierPathNode>(next_identifier, true);`);
                    this.emitter.emit(`auto& last_path_node_reference = parser_context.get_node_from_handle<IdentifierPathNode>(last_path_node);`);
                    this.emitter.emit(`auto& current_scoped_identifier = parser_context.get_node_from_handle<IdentifierPathNode>(scoped_identifier);`);
                    this.emitter.emit(`last_path_node_reference.next_segment = scoped_identifier;`);
                    this.emitter.emit(`last_path_node = scoped_identifier;`);
                });

                this.emitter.emit(`return scoped_identifier_head;`);
            });
        }

        emit_expect_literal_node() {
            this.emitter.emit_section_header("expect_literal_node");
            this.emitter.emit(`NodeHandle expect_literal_node(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => {
                this.emitter.see_current_token("current_token");
                this.emitter.emit(`switch (current_token.token_type)`);
                this.emitter.emit_scope(() => {
                    this.emitter.emit(`case TokenType::Char:`);
                    this.emitter.emit_scope(() => {
                        this.emitter.emit(`auto char_value = parser_context.get_current_char_value();`);
                        this.emitter.emit_create_node_assign("char_node", "CharLiteral", "char_value");
                        this.emitter.emit_return("char_node");
                    });

                    this.emitter.emit(`case TokenType::String:`);
                    this.emitter.emit_scope(() => {
                        this.emitter.emit_create_node_assign("string_node", "StringLiteral", "current_token.as<CLua::StringToken>()");
                        this.emitter.emit_return("string_node");
                    });

                    this.emitter.emit(`case TokenType::Numeric:`);
                    this.emitter.emit_scope(() => {
                        this.emitter.emit(`auto number_hint = parser_context.get_current_number_hint();`);
                        this.emitter.emit(`PAssert(number_hint.number_type != NumberType::None, "unexpected state, where number type has not been set despite token type being a literal numeric");`);
                        this.emitter.emit(`auto integer = parser_context.get_current_integer();`);
                        this.emitter.emit(`if (number_hint.number_type == NumberType::Float) {`);
                        this.emitter.step_indent();
                        this.emitter.emit(`auto fraction = parser_context.get_current_fraction();`);
                        this.emitter.emit(`return parser_context.create_node<NumberNode>(fraction + integer);`);
                        this.emitter.step_dedent();
                        this.emitter.emit(`} else if (number_hint.number_type == NumberType::Integer) {`);
                        this.emitter.step_indent();
                        this.emitter.emit(`return parser_context.create_node<IntegerLiteral>(integer);`);
                        this.emitter.step_dedent();
                        this.emitter.emit(`};`);
                    });

                    this.emitter.emit(`default:`);
                    this.emitter.emit_scope(() => {
                        this.emitter.emit(`auto unexpected_token = parser_context.create_node<UnexpectedTokenError>(current_token);`);
                        this.emitter.emit(`auto error = ParserError(current_token);`);
                        this.emitter.emit(`error.node_handle = unexpected_token;`);
                        this.emitter.emit(`return parser_context.emit_error(error);`);
                    });
                });

                this.emitter.emit_return("InvalidNode");
            });
        }

        emit_expect_atom() {
            this.emitter.emit_section_header("expect_atom");
            this.emitter.emit(`NodeHandle expect_atom(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => {
                this.emitter.emit(`if (parser_context.is_literal()) {`);
                this.emitter.step_indent();
                this.emitter.emit(`return expect_literal_node(parser_context);`);
                this.emitter.step_dedent();
                this.emitter.emit(`} else if(parser_context.is_symbol(SymbolKind::LParen)) {`);
                this.emitter.step_indent();
                this.emitter.emit(`return expect_group_expression(parser_context);`);
                this.emitter.step_dedent();
                this.emitter.emit(`} else {`);
                this.emitter.step_indent();
                this.emitter.emit(`return expect_scoped_identifier(parser_context);`);
                this.emitter.step_dedent();
                this.emitter.emit(`};`);
                this.emitter.emit_return("InvalidNode");
            });
        }

        emit_expression_delegates() {
            this.emitter.emit_section_header("expression_delegates");
            this.emitter.emit(`NodeHandle expect_unary_expression(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => { this.emitter.emit(`return Base::expect_atom(parser_context);`); });

            this.emitter.emit(`NodeHandle expect_expression(ParserContext& parser_context)`);
            this.emitter.emit_scope(() => { this.emitter.emit(`return expect_unary_expression(parser_context);`); });
        }

        emit_print_and_generate() {
            this.emitter.emit_section_header("print_and_generate");
            this.emitter.emit(`void Parser::print_node_tree(NodeHandle node_handle,Common::uint64 current_depth)`);
            this.emitter.emit_scope(() => {
                this.emitter.emit(`NodeHandleTag node_tag = node_handle.node_tag;`);
                this.emitter.emit(`PAssert(node_tag == NodeHandleTag::Error || node_tag == NodeHandleTag::Valid, "unexpected node handle tag type");`);
                this.emitter.emit(`if (node_tag == NodeHandleTag::Error) {`);
                this.emitter.step_indent();
                this.emitter.emit(`return print_error_node(node_handle,current_depth);`);
                this.emitter.step_dedent();
                this.emitter.emit(`} else if(node_tag == NodeHandleTag::Valid) {`);
                this.emitter.step_indent();
                this.emitter.emit(`return print_valid_node(node_handle,current_depth);`);
                this.emitter.step_dedent();
                this.emitter.emit(`};`);
                this.emitter.emit(`return;`);
            });

            this.emitter.emit(`NodeHandle Parser::generate_AST()`);
            this.emitter.emit_scope(() => {
                this.emitter.emit(`parser_context.get_next_token(); //fetch first token`);
                this.emitter.emit(`auto expression = Expression::expect_expression(parser_context);`);
                this.emitter.emit(`return expression;`);
            });
        }
    }

    const emitter = new BaseEmitter();
    const pe = new ParserEmitter(emitter);

    pe.emit_nodes_list();
    pe.emit_consume_symbol();
    pe.emit_expect_identifier();
    pe.emit_expect_group_expression();
    pe.emit_expect_scoped_identifier();
    pe.emit_expect_literal_node();
    pe.emit_expect_atom();
    pe.emit_expression_delegates();
    pe.emit_print_and_generate();

    console.log(emitter.code);
};

function emit_ast_nodes()
{

};

export function build_parser()
{
    //process nodes and then pattern, then emit the code
    emit_parser_cpp();
};
