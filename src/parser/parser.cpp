#include "parser.hpp"

namespace ASTParser{
    using namespace CLuaNodes;
    using namespace SymbolClassifier;

    using TokenType = Util::TokenType;
    using NumberBase = Util::NumberBase;
    using NumberType = Util::NumberType;
    using Source = Util::Source;

    namespace Expression {
        NodeHandle get_assignement_expression(ParserContext& parser_context)
        {

        };

        NodeHandle get_expression(ParserContext& parser_context)
        {
            //ternary op should be processed here first but 
            //I am not intending to keep them for now

            //process ternary op, then binary op, then unary op
        };
    };

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
    }

    NodeHandle get_action(ParserContext& parser_context)
    {
        /*/
        for now get_action is expected to be
        consisting of expression/simple expression, 
        because of this other elements like declaration/statements/metalang will not
        be included for now in this get_action body

        also it's the first element in the graph that will
        be implemented in memory in linear manager
        /*/
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

     if (action_chain == InvalidNode)
     {
        std::cout << "Error occured. Parsing proccess was unsuccessful" << std::endl;
     };

     return action_chain;
    }
}