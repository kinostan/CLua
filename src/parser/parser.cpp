#include "parser.hpp"

#include <base.hpp>

namespace ASTParser{
    template <typename T, typename... Ts>
    constexpr bool is_one_of(const T& value, Ts... values)
    {   
        return ((value == values) || ...); // fold expression (C++17+)
    }

    using namespace CLuaNodes;
    using namespace SymbolClassifier;

    using TokenType = Util::TokenType;
    using NumberBase = Util::NumberBase;
    using NumberType = Util::NumberType;
    using Source = Util::Source;

    namespace Expression {
        NodeHandle expect_expression(ParserContext& parser_context);
    };

    namespace Base {
        NodeHandle expect_group_expression(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();

            PAssert(
                parser_context.is_symbol(SymbolKind::LParen),  
                "guessed token type doesn't fit the requirements '(' expected"
            );

            parser_context.get_next_token();
            auto expression_node = Expression::expect_expression(parser_context);
            auto node_tag = get_node_tag_from_handle(expression_node);

            auto return_node = InvalidNode;

            if (node_tag == NodeHandleTag::Valid) [[likely]] {
                auto group_node = parser_context.create_node<GroupExpression>(expression_node);
                auto next_token = parser_context.see_current_token();

                if (parser_context.is_symbol(SymbolKind::RParen))
                {
                    parser_context.get_next_token();
                    return group_node;
                } else {
                    /*
                    Unclosed group expression error
                    */
                    
                    ParserError parser_error = ParserError(current_token,next_token);
                    parser_error.node_handle = parser_context.create_node<UnclosedBlockError>();

                    return parser_context.emit_error(parser_error);
                };
            } else if(node_tag == NodeHandleTag::Error) {
                return expression_node;
            } else {
                PAssert(false, 
                 "unexpected node tag being returned, this function either returns Valid/Error node tag handle"
                );
                return return_node;
            };
        };

        NodeHandle expect_identifier(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();

            if (parser_context.is_identifier())
            {
                parser_context.get_next_token(); 
                return parser_context.create_node<Identifier>(
                    current_token.as<Util::IdentifierToken>()
                );
            } else {
                auto parser_error = ParserError(current_token);
                parser_error.node_handle = parser_context.create_node<UnexpectedTokenError>(
                    current_token
                );    
                auto unexpected_token = parser_context.emit_error(parser_error);

                return unexpected_token;
            };
            return InvalidNode;
        };

        NodeHandle get_scoped_identifier(ParserContext& parser_context)
        {
            auto optional_global_scope = parser_context.consume_symbol(SymbolKind::DoubleColon); 
            auto identifier_token = parser_context.see_current_token();

            if (identifier_token.token_type != TokenType::Identifier)
            {
                return set_node_state_for_handle(0,NodeHandleTag::NoPattern);
            };

            auto identifier_path_head = parser_context.create_node<IdentifierPathNode>(identifier_token,optional_global_scope);
            auto& node_reference = parser_context.get_node_from_handle<IdentifierPathNode>(identifier_path_head);

            auto previous_node_handle = identifier_path_head;

            while (!parser_context.has_reached_end())
            {
                auto local_scope = parser_context.consume_symbol(SymbolKind::DoubleColon);
                if (!local_scope)
                {
                    break;
                };
                identifier_token = parser_context.see_current_token();
                
                if (identifier_token.token_type == TokenType::Identifier) [[unlikely]]
                {
                    auto unexpected_token = parser_context.create_node<UnexpectedTokenError>(identifier_token);
                    auto error = ParserError(identifier_token);
                    error.node_handle = unexpected_token;
                    return parser_context.emit_error(error);
                };

                parser_context.get_next_token();
                auto current_node_handle = parser_context.create_node<IdentifierPathNode>(identifier_token,true);

                auto& current_node_reference = parser_context.get_node_from_handle<IdentifierPathNode>(current_node_handle);
                auto& previous_node_reference = parser_context.get_node_from_handle<IdentifierPathNode>(previous_node_handle);

                previous_node_reference.next_segment = current_node_handle;
                previous_node_handle = current_node_handle;
            };
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
                        current_token.as<Util::StringToken>()
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
            auto cursor_record = parser_context.get_lexer().record_cursor();

            auto scoped_identifier = get_scoped_identifier(parser_context);
            auto result_tag = get_node_tag_from_handle(scoped_identifier);

            if (is_one_of(result_tag,NodeHandleTag::Error,NodeHandleTag::Valid))
            {
                return scoped_identifier;
            };
            
            parser_context.get_lexer().set_cursor(cursor_record);

            auto& literal = scoped_identifier;
            literal = expect_literal_node(parser_context);

            return literal;
        };
    };
       
    namespace Expression {
        NodeHandle expect_expression(ParserContext& parser_context)
        {

        };
    };

    void Parser::print_node_tree(NodeHandle node_handle,Util::uint64 current_depth){
        NodeHandleTag node_tag = get_node_tag_from_handle(node_handle);

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
        auto expression = Expression::expect_expression(parser_context);
        print_node_tree(expression);
        return expression;
    }
}