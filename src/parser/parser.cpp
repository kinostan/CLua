#include "parser.hpp"

namespace ASTParser{
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
            SymbolKind symbol;
            switch (symbol)
            {
            case SymbolKind::Equal: case SymbolKind::PlusEqual: case SymbolKind::MinusEqual: 
            case SymbolKind::SlashEqual: case SymbolKind::StarEqual:
                return true;
            default:
                return false;
            }
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

        NodeHandle process_unary_expression(ParserContext& parser_context)
        {

        };

        NodeHandle process_additive_expression(ParserContext& parser_context)
        {
            auto top = process_unary_expression(parser_context);
            auto most_left_expression = top;
            auto process_next = is_additive_symbol_token(parser_context);

            while (process_next)
            {
                most_left_expression = process_unary_expression(parser_context);
                process_next = is_additive_symbol_token(parser_context);
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
            if (parser_context.is_error_node(binary_expression_head))
            {
                std::cout << "failed to parse an expression" << std::endl;
            };
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