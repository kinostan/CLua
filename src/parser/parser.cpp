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
            
    void Parser::print_node_tree(NodeHandle node_handle,size_t current_depth){
        if (parser_context.is_error_node(node_handle))
        {
            print_error_node(node_handle,current_depth);
        } else {
            print_valid_node(node_handle,current_depth);
        };
    };

    NodeHandle Parser::generate_AST(){
        
    }
}