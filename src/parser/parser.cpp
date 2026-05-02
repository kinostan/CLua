#include "parser.hpp"

#include <common/clua/tokens.hpp>
#include <common/base.hpp>

namespace ASTParser{
    template <typename T, typename... Ts>
    constexpr bool is_one_of(const T& value, Ts... values)
    {   
        return ((value == values) || ...); // fold expression (C++17+)
    }

    using namespace CLuaNodes;

    using SymbolKind = CLua::SymbolKind;

    using TokenType = CLua::TokenType;
    using NumberBase = CLua::NumberBase;
    using NumberType = CLua::NumberType;
    using Source = CLua::Source;

    namespace Expression {
        NodeHandle expect_expression(ParserContext& parser_context);
    };

    namespace Base {
        bool consume_symbol(ParserContext& parser_context, CLua::SymbolKind expected_symbol)
        {
            auto current_token = parser_context.see_current_token();

            auto can_be_consumed = current_token.token_type == TokenType::Symbol && parser_context.get_current_symbol() == expected_symbol;
        
            if (!can_be_consumed) [[unlikely]]
            {
                return false;
            };

            parser_context.get_next_token();

            return true;
        };

        NodeHandle expect_identifier(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();
            auto is_identifier = parser_context.is_identifier();

            if (!is_identifier) [[unlikely]]
            {
                auto error_node = parser_context.create_node<UnexpectedTokenError>(
                    current_token
                );
                ParserError error = ParserError(current_token);
                return parser_context.emit_error(error);
            };

            parser_context.get_next_token();
            auto identifier_node = parser_context.create_node<Identifier>(
                current_token.as<CLua::IdentifierToken>()
            );

            return identifier_node;
        };

        NodeHandle expect_group_expression(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();

            if (!parser_context.is_symbol(SymbolKind::LParen))
            {
                return NoPatternNode;
            };

            parser_context.get_next_token();
            auto expression_node = Expression::expect_expression(parser_context);

            auto return_node = InvalidNode;

            if (expression_node.node_tag == NodeHandleTag::Valid) [[likely]] {
                auto next_token = parser_context.see_current_token();

                if (parser_context.is_symbol(SymbolKind::RParen))
                {
                    parser_context.get_next_token();
                    return parser_context.create_node<GroupExpression>(expression_node);
                } else {
                    ParserError parser_error = ParserError(current_token,next_token);
                    parser_error.node_handle = parser_context.create_node<UnclosedBlockError>();

                    return parser_context.emit_error(parser_error);
                };
            } else if(expression_node.node_tag == NodeHandleTag::Error) {
                return expression_node;
            } else {
                PAssert(false, 
                 "unexpected node tag being returned, this function either returns Valid/Error node tag handle"
                );
                return return_node;
            };
        };

        NodeHandle expect_scoped_identifier(ParserContext& parser_context)
        {
            auto optional_global_scope = consume_symbol(parser_context,SymbolKind::DoubleColon); 
            auto identifier = expect_identifier(parser_context);

            if (identifier.node_tag != NodeHandleTag::Valid)
            {
                return identifier;
            };
    
            auto scoped_identifier_head = parser_context.create_node<IdentifierPathNode>(
                identifier,
                optional_global_scope
            );

            auto last_path_node = scoped_identifier_head;

            while (!parser_context.has_reached_end())
            {
                auto scope_enabled = consume_symbol(parser_context, SymbolKind::DoubleColon);

                if (!scope_enabled)
                {
                    break;
                }
                
                auto next_identifier = expect_identifier(parser_context);

                if (next_identifier.node_tag != NodeHandleTag::Valid)
                {
                    return next_identifier;
                };

                auto scoped_identifier = parser_context.create_node<IdentifierPathNode>(
                    next_identifier,
                    true
                );

                auto& last_path_node_reference = parser_context.get_node_from_handle<IdentifierPathNode>(last_path_node);
                auto& current_scoped_identifier = parser_context.get_node_from_handle<IdentifierPathNode>(scoped_identifier);

                last_path_node_reference.next_segment = scoped_identifier;
                last_path_node = scoped_identifier;
            };

            return scoped_identifier_head;
        };

        NodeHandle expect_literal_node(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();

            switch (current_token.token_type)
            {
                case TokenType::Char:
                {
                    auto char_value = parser_context.get_current_char_value();
                    auto char_node = parser_context.create_node<CharLiteral>(char_value);
                    return char_node;
                };
                case TokenType::String:
                {
                    auto string_node = parser_context.create_node<StringLiteral>(
                        current_token.as<CLua::StringToken>()
                    );

                    return string_node;
                };
                case TokenType::Numeric:
                {
                    auto number_hint = parser_context.get_current_number_hint();
                    
                    PAssert(
                        number_hint.number_type != NumberType::None,
                        "unexpected state, where number type has not been set despite"
                        "token type being a literal numeric"
                    )

                    auto fraction = parser_context.get_current_fraction();
                    auto integer = parser_context.get_current_integer();

                    if (number_hint.number_type == NumberType::Float)
                    {
                        auto num_val = fraction + integer;
                        
                    } else if (number_hint.number_type == NumberType::Integer)
                    {
                        auto integer_node = parser_context.create_node<IntegerLiteral>(integer);
                    };
                };  
                default:
                {
                    auto unexpected_token = parser_context.create_node<UnexpectedTokenError>(current_token);
                    auto error = ParserError(current_token);
                    error.node_handle = unexpected_token;
                    return parser_context.emit_error(error);
                }
            }

            return InvalidNode;
        };

        NodeHandle expect_atom(ParserContext& parser_context)
        {
            if (parser_context.is_literal())
            {
                return expect_literal_node(parser_context);
            } else if(parser_context.is_symbol(SymbolKind::LParen)) {
                return expect_group_expression(parser_context);
            }
            else
            {
                return expect_scoped_identifier(parser_context);
            };

            return InvalidNode;
        };
    };
       
    namespace Expression {
        bool is_prefix_unary(ParserContext& parser_context)
        {
            
        };

        NodeHandle expect_unary_expression(ParserContext& parser_context)
        {
            return Base::expect_atom(parser_context);
        };

        NodeHandle expect_expression(ParserContext& parser_context)
        {
            return expect_unary_expression(parser_context);
        };
    };

    void Parser::print_node_tree(NodeHandle node_handle,Common::uint64 current_depth){
        NodeHandleTag node_tag = node_handle.node_tag;

        PAssert(
            node_tag == NodeHandleTag::Error || node_tag == NodeHandleTag::Valid,
            "unexpected node handle tag type"
        );

        if (node_tag == NodeHandleTag::Error)
        {
            return print_error_node(node_handle,current_depth);
        } else if(node_tag == NodeHandleTag::Valid) {
            return print_valid_node(node_handle,current_depth);
        }; 

        return;
    };

    NodeHandle Parser::generate_AST(){
        parser_context.get_next_token(); //fetch first token
        auto expression = Expression::expect_expression(parser_context);
        return expression;
    }
}