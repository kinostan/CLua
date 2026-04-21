#include "parser.hpp"
#include "index.cpp"

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
                current_token.token_type == TokenType::Symbol && 
                parser_context.get_current_symbol() == SymbolKind::LParen,  
                "guessed token type doesn't fit the requirements '(' expected"
            );

            parser_context.get_next_token();
            auto expression_node = expect_group_expression(parser_context);
            auto node_tag = get_node_tag_from_handle(expression_node);

            auto return_node = InvalidNode;

            if (node_tag == NodeHandleTag::Valid) [[likely]] {
                auto group_node = parser_context.create_node<GroupExpression>(expression_node);
                return_node = group_node;

                current_token = parser_context.see_current_token();

                if (current_token.token_type == TokenType::Symbol && parser_context.get_current_symbol() == SymbolKind::RParen)
                {
                    parser_context.get_next_token();
                    return group_node;
                } else {
                    /*
                    Unclosed group expression error
                    */
                    ParserError parser_error = ParserError();
                    parser_error.error_node_type = NodeType::Error;
                    parser_error.node_handle = InvalidNode;

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
            
        };

        NodeHandle expect_identifier_path(ParserContext& parser_context)
        {
            
        };

        NodeHandle get_atom(ParserContext& parser_context)
        {
            
        };
    };
            
    void Parser::print_node_tree(NodeHandle node_handle,size_t current_depth){
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
        
    }
}