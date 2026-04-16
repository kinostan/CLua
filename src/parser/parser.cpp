#include "parser.hpp"

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

        bool is_expression_termination_token(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();
            if (current_token.token_type != TokenType::Symbol)
            {
                return false;
            };

            auto current_symbol = parser_context.get_current_symbol();

            switch (current_symbol)
            {
            case SymbolKind::Semicolon: case SymbolKind::RBrace: case SymbolKind::RBracket: case SymbolKind::RParen: case SymbolKind::Comma:
                return true;
            default:
                return false;
            }
        };

        bool is_assignement_symbol_token(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();
            SymbolKind symbol = parser_context.get_current_symbol();
            switch (symbol)
            {
            case SymbolKind::Equal: case SymbolKind::PlusEqual: case SymbolKind::MinusEqual: 
            case SymbolKind::SlashEqual: case SymbolKind::StarEqual:
                return true;
            default:
                return false;
            }
        };

        bool is_keyword_identifier(ParserContext& parser_context)
        {
            return parser_context.get_current_keyword() != KeywordClassifier::Keyword::Unknown;
        };

        bool is_additive_symbol_token(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();
            SymbolKind symbol;
            switch (symbol)
            {
            case SymbolKind::Plus: case SymbolKind::Minus:
                return true;
            default:
                return false;
            }
        };

        UnOperationType symbol_kind_to_unary_operation_type(SymbolKind symbol)
        {

        };

        BinOperationType symbol_kind_to_binary_operation_type(SymbolKind symbol)
        {

        };

        TernarnyOperationType symbol_kind_to_ternary_operation_type(SymbolKind symbol)
        {

        };

        inline UnOperationType get_unary_prefix_operator(ParserContext& parser_context)
        {
            auto current_symbol = parser_context.get_current_symbol();
            auto operation_type = symbol_kind_to_unary_operation_type_prefix(current_symbol);

            if (operation_type != UnOperationType::None)
            {
                parser_context.get_next_token();
            };

            return operation_type;
        };

        inline UnOperationType get_unary_postfix_operator(ParserContext& parser_context)
        {
            auto current_symbol = parser_context.get_current_symbol();
            auto operation_type = symbol_kind_to_unary_operation_type_postfix(current_symbol);

            if (operation_type != UnOperationType::None)
            {
                parser_context.get_next_token();
            };

            return operation_type;
        };

        NodeHandle process_local_expression(ParserContext& parser_context)
        {

        };

        NodeHandle process_identifier_path(ParserContext& parser_context)
        {

        }

        NodeHandle process_atom(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();

            PAssert(
                is_one_of(current_token.token_type,
                    TokenType::Identifier,
                    TokenType::Symbol,
                    TokenType::Char,
                    TokenType::String,
                    TokenType::Numeric
                )//basically if it's an atom type first barrier check 
                , "unexpected token type in process_atom() function"
            )

            auto simple_node = InvalidNode;

            /*/
            atom either can be a:
            IdentifierPath where it means
            ("::"? identifier)("." .. identifier | "::" .. identifier)* 
            
            where identifier must not be a keyword.
            /*/ 

            if (current_token.token_type == TokenType::Identifier)
            {


            } else {
                //plain literal

            };
            
            /*/

            When identifier is a keyword and is used for expressions then 
            it's legal, it's more less understood as a local expression which is initialized 
            in a different way

            A literal of any type

            or a local expression starting with "(" symbol
            /*/
        };

        NodeHandle process_keyword_expression(ParserContext& parser_context)
        {
            using KeywordType = KeywordClassifier::Keyword;
            auto keyword_type = parser_context.get_current_keyword();
            switch (keyword_type)
            {
            case KeywordType::Sizeof:
                //size of expression
                return;
            case KeywordType::StaticCast:
                return;  
            case KeywordType::ReinterpretCast:
                return;
            default:
                ParserError error = ParserError();
                error.error_code = ParserErrorCode::InvalidExpression;
                error.node_handle = InvalidNode;

                return parser_context.emit_error(error);
            }
        };

        NodeHandle process_postfix_unary_expression(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();
            auto simple_node = InvalidNode;
            auto token_type = current_token.token_type;

            switch (token_type)
            {
                case TokenType::Symbol:
                {
                    auto current_symbol = parser_context.get_current_symbol();
                    if (current_symbol == SymbolKind::LParen)
                    {
                        simple_node = process_local_expression(parser_context);
                    } else if(current_symbol == SymbolKind::DoubleColon) 
                    {
                        simple_node = process_atom(parser_context);
                    }
                    else {
                        PAssert(
                            false, 
                            "unhandled case"
                        )
                    }
                    break;
                };
                case TokenType::Char: case TokenType::String: case TokenType::Numeric: case TokenType::Identifier:
                {
                    if (token_type == TokenType::Identifier && is_keyword_identifier(parser_context))
                    {
                        simple_node = process_keyword_expression(parser_context);
                    } else {
                        simple_node = process_atom(parser_context);
                    };
                    break;
                };
                default: 
                {
                    ParserError error = ParserError();
                    error.error_code = ParserErrorCode::InvalidExpression;
                    error.node_handle = InvalidNode;

                    return parser_context.emit_error(error);
                };
            };

            if (parser_context.is_error_node(simple_node))
            {
                return simple_node; 
            };

            auto top_node = simple_node;    
            //unfinished
        };
    
        NodeHandle process_unary_expression(ParserContext& parser_context)
        {
            auto current_token = parser_context.see_current_token();
            switch (current_token.token_type)
            {
                case TokenType::Symbol:
                {
                    auto current_symbol = parser_context.get_current_symbol();

                    if (current_symbol == SymbolKind::LParen || current_symbol == SymbolKind::DoubleColon)
                    {
                        return process_postfix_unary_expression(parser_context);
                    };
                    
                    auto return_node = InvalidNode;
                    auto operation_type = get_unary_prefix_operator(parser_context);

                    if (operation_type != UnOperationType::None)
                    {
                        auto unary_expression_node = parser_context.create_node<UnaryNode>(operation_type);
                        auto& unary_node = parser_context.get_node_from_handle<UnaryNode>(unary_expression_node);

                        unary_node.node = process_unary_expression(parser_context);

                        return unary_expression_node;
                    }
                    else {
                        ParserError error = ParserError();
                        error.error_code = ParserErrorCode::InvalidExpression;
                        error.node_handle = InvalidNode;
                        return parser_context.emit_error(error);
                    };  

                    PAssert(
                        return_node != InvalidNode,
                        "return_node can't be InvalidNode (const)"
                    )

                    return return_node;
                };
                case TokenType::Char: case TokenType::String: case TokenType::Numeric: case TokenType::Identifier:
                {
                    return process_postfix_unary_expression(parser_context);
                };
                default:
                {   
                    ParserError error = ParserError();//should be replaced with a specialzed error node and 
                    //attached to error with more information
                    error.error_code = ParserErrorCode::InvalidExpression;//unrecognized symbol
                    error.node_handle = InvalidNode;

                    return parser_context.emit_error(error);
                };
            }
        };

        NodeHandle process_additive_expression(ParserContext& parser_context)
        {
            auto top = InvalidNode;

            auto most_left_expression = process_unary_expression(parser_context);//should be replaced with multiplicative soon enough
            if (parser_context.is_error_node(most_left_expression))
            {
                return most_left_expression;
            };

            top = most_left_expression;

            auto process_next = is_additive_symbol_token(parser_context);
            while (process_next)
            {
                auto operator = get_
                auto right_side = process_unary_expression(parser_context);
                if (parser_context.is_error_node(most_left_expression))
                {
                    return most_left_expression;
                };

                auto new_top = parser_context.create_node<BinaryNode>(BinOperationType::Add);

            }

            if (!is_expression_termination_token(parser_context))
            {
                ParserError new_error = ParserError();
                new_error.error_code = ParserErrorCode::InvalidExpression;
                new_error.node_handle = InvalidNode;
                return parser_context.emit_error(new_error);
            };
            
            return top;
        };

        NodeHandle process_assignement_expression(ParserContext& parser_context)
        {
            auto top = process_additive_expression(parser_context);
            auto most_left_expression = top;
            auto process_next = is_assignement_symbol_token(parser_context);

            while (process_next)
            {
                most_left_expression = process_additive_expression(parser_context);
                process_next = is_assignement_symbol_token(parser_context);
            }

            if (!is_expression_termination_token(parser_context))
            {
                ParserError new_error = ParserError();
                new_error.error_code = ParserErrorCode::InvalidExpression;
                new_error.node_handle = top;
                return parser_context.emit_error(new_error);
            };
            
            return top;
        };

        NodeHandle get_expression(ParserContext& parser_context)
        {
            auto binary_expression_head = process_assignement_expression(parser_context);  
            return binary_expression_head;
        };
    };

    /*
    namespace Statement {  
        void consume_statement()
        {

        };
    }

    namespace Declaration {
        void consume_declaration()
        {

        };
    }

    namespace Metalanguage {
        NodeHandle consume_metalanguage()
        {

        };
    }*/

    NodeHandle get_action(ParserContext& parser_context)
    {
        NodeHandle expression_node = Expression::get_expression(parser_context);  
        if (parser_context.is_error_node(expression_node))
        {
            return InvalidNode;
        };

        NodeHandle action_handle = parser_context.create_node<ActionNode>(expression_node);
        return action_handle;
    };
    
    NodeHandle get_first_valid_action(ParserContext& parser_context)
    {
        while (!parser_context.has_reached_end())
        {
            auto action_handle = get_action(parser_context);
            if (!parser_context.is_error_node(action_handle))
            {
                return action_handle;
            };
        };
        return InvalidNode;
    };

    NodeHandle get_action_chain(ParserContext& parser_context) 
    {
        auto action_handle_chain_head = get_first_valid_action(parser_context);
        if (parser_context.is_error_node(action_handle_chain_head))
        {
            return action_handle_chain_head;
        };

        auto action_handle = action_handle_chain_head;
        while (!parser_context.has_reached_end())
        {
            auto new_action_handle = get_action(parser_context);
            if(parser_context.is_error_node(new_action_handle))
            {
                continue; 
            };

            auto& action = parser_context.get_node_from_handle<ActionNode>(action_handle);
            action.next_action = new_action_handle;
            action_handle = new_action_handle;
        };
        return action_handle_chain_head;
    };

    NodeHandle Parser::generate_AST(){
     auto action_chain = get_action_chain(parser_context);

     if (parser_context.is_error_node(action_chain))
     {
        std::cout << "Totally failed to compile the input" << std::endl;
     };

     return action_chain;
    }
}