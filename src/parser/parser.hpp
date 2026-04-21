#pragma once

#include "node_manager.hpp"

#include <lexer/lexer.hpp>

#include <ast_nodes/index.hpp>
#include <ast_nodes/error_nodes.hpp>

#include <iostream>
#include <vector>

namespace ASTParser{

    using NodeType = CLuaNodes::NodeType;
    using NodeHandle = CLuaNodes::NodeHandle;
    using BaseNode = CLuaNodes::BaseNode;
    //AST-concept-1 should only parse math expressions and evaluate them immedieatly i
    class ParserError {
        public:
        NodeType error_node_type = NodeType::Invalid;
        NodeHandle node_handle = InvalidNode;
        //TokenSpan error_span;
    };

    class ParserContext{
        bool has_reached_eof = false;

        Util::Lexer lexer;
        Util::NodeManager node_manager;
        
        Util::TokenGeneric current_token = static_cast<Util::TokenGeneric>(Util::NoToken()); //last acquired token really, but it points to current token in a way
        Util::TokenGeneric last_token = static_cast<Util::TokenGeneric>(Util::NoToken()); //last acquired token really, but it points to current token in a way

        std::vector<ParserError> error_list;

        Util::TokenGeneric get_next_non_neutral_token()
        {
            Util::TokenGeneric token;

            do
            {
                token = lexer.process_next_token();
            }
            while (token.token_type == Util::TokenType::Comment ||
                token.token_type == Util::TokenType::NewLine ||
                token.token_type == Util::TokenType::Whitespace);
                
            return token;
        }

        public:

        ParserContext(Util::Source& source): 
        lexer(source), node_manager(source.source_size / 3)
        {};


        template <typename Node>
        requires (std::derived_from<Node,BaseNode>)
        inline bool is_node_type(NodeHandle node_handle,NodeType expected_node_type)
        {
            //function is redundant and should not exist in the future or it's use should be
            //turned down
            auto& node = get_node_from_handle<Node>(node_handle);
            return node.node_type == expected_node_type;
        };

        template<typename Node, typename... Args>
        requires (std::derived_from<Node,BaseNode> && std::is_constructible_v<Node, Args...>)
        inline NodeHandle create_node(Args&&... args)
        {
            return node_manager.create_node<Node>(std::forward(args)...);
        };

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>) 
        inline Node* get_node_pointer_from_handle(NodeHandle node_handle){
            PAssert(
                !is_error_node(node_handle),
                "node handle must be valid to be casted from node handle to node pointer"
            )
            return node_manager.get_node_pointer_from_handle<Node>(node_handle);
        }

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>)
        inline Node& get_node_from_handle(NodeHandle node_handle)
        {
            PAssert(
                !is_error_node(node_handle),
                "node handle must be valid for function to be able to return a reference of a node"
            )
            return *get_node_pointer_from_handle<Node>(node_handle);
        };

        bool has_reached_end()
        {
            return has_reached_eof;
        };

        Util::Lexer& get_lexer()
        {
            return lexer;
        }

        Util::TokenGeneric get_next_token()
        {
            auto next_token = get_next_non_neutral_token();
            last_token = current_token;
            current_token = next_token;
            return next_token;
        };

        Util::TokenGeneric see_current_token()
        {
            return current_token;
        };

        Util::TokenGeneric get_last_token() {
            return last_token;
        };

        Util::TokenGeneric peek_next_token()
        {
            return lexer.peek_next_token();
        };

        Util::NumberHint get_current_number_hint()
        {
            return lexer.get_current_number_hint();
        };

        Util::Error get_current_error()
        {
            return lexer.get_current_error();
        };

        SymbolClassifier::SymbolKind get_current_symbol()
        {
            if (current_token.token_type != Util::TokenType::Symbol) [[unlikely]]
            {
                return SymbolClassifier::SymbolKind::Unknown;
            };
            return lexer.get_current_symbol();
        };

        KeywordClassifier::Keyword get_current_keyword()
        {
            return lexer.get_current_keyword();
        };

        NodeHandle emit_error(ParserError& parser_error)
        {
            //due to error spans, then it means that
            //the current assertion no longer fits under the requirements.
            //it should keep track of error spans to monitor overlaps and report
            //if they happen in the code
            auto error_id = error_list.size();
            auto error_handle = CLuaNodes::create_error_node_handle(error_id);

            error_list.push_back(parser_error);

            return error_handle;
        };
    };

    class Parser{
        ParserContext parser_context;
        public:

        Parser(Util::Source& source): parser_context(source)
        {};        

        NodeHandle generate_AST();

        private:

        void print_node_tree(NodeHandle,size_t);
        
        void print_error_node(NodeHandle node_handle,size_t current_depth)
        {
            std::cout << "Error Node ID: " << node_handle << std::endl;
        };

        std::string_view get_token_text(const Util::TokenGeneric& token)
        {
            auto& source = parser_context.get_lexer().get_lexer_context().source;
            return source.slice_string(token.offset, token.length);
        }

        const char* separator_to_string(CLuaNodes::IdentifierPathSeparator separator)
        {
            switch (separator)
            {
            case CLuaNodes::IdentifierPathSeparator::None:
                return "";
            case CLuaNodes::IdentifierPathSeparator::Dot:
                return ".";
            case CLuaNodes::IdentifierPathSeparator::DoubleColon:
                return "::";
            default:
                return "?";
            }
        }

        void print_valid_node(NodeHandle node_handle,size_t current_depth)
        {
            using namespace CLuaNodes;
            auto* node_ptr = parser_context.get_node_pointer_from_handle<BaseNode>(node_handle);
            auto node_type = node_ptr->node_type;
            std::string indent(current_depth * 2, ' ');
            
            switch (node_type)
            {
            case NodeType::CharLiteral:
                std::cout << indent << "CharLiteral" << std::endl;
                break;
            case NodeType::StringLiteral:
                std::cout << indent << "StringLiteral" << std::endl;
                break;
            case NodeType::NumberLiteral: {
                auto* number_node = static_cast<CLuaNodes::NumberNode*>(node_ptr);
                std::cout << indent << "NumberLiteral: " << number_node->value << std::endl;
                break;
            }
            case NodeType::IntegerLiteral: {
                auto* integer_node = static_cast<CLuaNodes::IntegerLiteral*>(node_ptr);
                std::cout << indent << "IntegerLiteral: " << integer_node->value << std::endl;
                break;
            }
            case NodeType::Identifier: {
                auto* identifier_node = static_cast<CLuaNodes::Identifier*>(node_ptr);
                std::cout << indent << "Identifier: " << get_token_text(identifier_node->identifier_token) << std::endl;
                break;
            }
            case NodeType::IdentifierPath: {
                auto* identifier_path_node = static_cast<CLuaNodes::IdentifierPathNode*>(node_ptr);
                std::cout << indent << "IdentifierPath: " << get_token_text(identifier_path_node->identifier_token) << std::endl;

                auto next_segment_handle = identifier_path_node->next_segment;
                while (next_segment_handle != InvalidNode)
                {
                    auto* segment_node = parser_context.get_node_pointer_from_handle<CLuaNodes::IdentifierPathNode>(next_segment_handle);
                    std::cout << indent << "  " << separator_to_string(segment_node->separator_from_previous)
                              << " " << get_token_text(segment_node->identifier_token) << std::endl;
                    next_segment_handle = segment_node->next_segment;
                }
                break;
            }
            case NodeType::GroupExpression: {
                std::cout << "GroupExpression is not handled yet" << std::endl;
                break;
            }
            case NodeType::Action: {
                std::cout << indent << "Action" << std::endl;
                auto* action_node = static_cast<CLuaNodes::ActionNode*>(node_ptr);
                print_node_tree(action_node->action_description, current_depth + 1);
                if (action_node->next_action != InvalidNode) {
                    print_node_tree(action_node->next_action, current_depth + 1);
                }
                break;
            }
            case NodeType::Expression: {
                std::cout << indent << "Expression" << std::endl;
                auto* expression_node = static_cast<CLuaNodes::ExpressionNode*>(node_ptr);
                print_node_tree(expression_node->expression_node_handle, current_depth + 1);
                break;
            }
            case NodeType::UnaryExpression: {
                std::cout << indent << "UnaryExpression" << std::endl;
                auto* unary_node = static_cast<CLuaNodes::UnaryNode*>(node_ptr);
                print_node_tree(unary_node->node, current_depth + 1);
                break;
            }
            case NodeType::BinaryExpression: {
                std::cout << indent << "BinaryExpression" << std::endl;
                auto* binary_node = static_cast<CLuaNodes::BinaryNode*>(node_ptr);
                print_node_tree(binary_node->left, current_depth + 1);
                print_node_tree(binary_node->right, current_depth + 1);
                break;
            }
            case NodeType::TernaryExpression: {
                std::cout << indent << "TernaryExpression" << std::endl;
                auto* ternary_node = static_cast<CLuaNodes::TernaryNode*>(node_ptr);
                print_node_tree(ternary_node->left, current_depth + 1);
                print_node_tree(ternary_node->center, current_depth + 1);
                print_node_tree(ternary_node->right, current_depth + 1);
                break;
            }
            default:
                std::cout << indent << "Unknown Node Type: " << static_cast<int>(node_type) << std::endl;
                break;
            }
        };

        public:
        void print_node_tree(NodeHandle node_handle) {
            return print_node_tree(node_handle,0);
        };
    };
}