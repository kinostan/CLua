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

            if (node_tag == NodeHandleTag::Valid) [[likely]] {
                auto group_node = parser_context.create_node<GroupExpression>(expression_node);
                return group_node;
            } else if(node_tag == NodeHandleTag::Error) {
                return expression_node;
            } else {
                PAssert(false, 
                 "unexpected node tag being returned, this function either returns Valid/Error"
                );
                return InvalidNode;
            };
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