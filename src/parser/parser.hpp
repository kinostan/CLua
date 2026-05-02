#pragma once

#include <common/base.hpp>
#include "node_manager.hpp"

#include <clua_lexer/lexer.hpp>

#include <ast_nodes/base.hpp>
#include <ast_nodes/error_nodes.hpp>

#include <iostream>
#include <vector>

namespace ASTParser{

    using NodeType = CLuaNodes::NodeType;
    using NodeHandleTag = CLuaNodes::NodeHandleTag;
    using NodeHandle = CLuaNodes::NodeHandle;
    using BaseNode = CLuaNodes::BaseNode;

    using TokenGeneric = CLua::TokenGeneric;
    using TokenSpan = CLua::TokenSpan;
    using TokenType = CLua::TokenType;
    
    const auto InvalidNode = CLuaNodes::InvalidNode;
    
    //AST-concept-1 should only parse math expressions and evaluate them immedieatly i
    class ParserError {
        public:

        ParserError(TokenGeneric start, TokenGeneric end): token_span(TokenSpan(start,end))
        {};
        ParserError(TokenGeneric only_token): token_span(TokenSpan(only_token,only_token))
        {};

        TokenSpan token_span;
        NodeHandle node_handle = InvalidNode;
    };

    struct ParserState {
        bool has_reached_eof = false;     
        CLua::TokenGeneric current_token; 
        CLua::TokenGeneric last_token;
        
        CLua::LexerState lexer_state;
    };

    class ParserContext{
        bool has_reached_eof = false;

        CLua::Lexer lexer;
        CLuaNodes::NodeManager node_manager;
        
        CLua::TokenGeneric current_token = static_cast<CLua::TokenGeneric>(CLua::NoToken()); //last acquired token really, but it points to current token in a way
        CLua::TokenGeneric last_token = static_cast<CLua::TokenGeneric>(CLua::NoToken()); //last acquired token really, but it points to current token in a way

        std::vector<ParserError> error_list;

        void emit_lexer_error_for_token(const CLua::TokenGeneric& error_token)
        {
            ParserError parser_error = ParserError(error_token,error_token);
            auto lexer_error = get_current_error();
            parser_error.node_handle = create_node<CLuaNodes::LexerErrorNode>(error_token, lexer_error);
            emit_error(parser_error);
        }

        CLua::TokenGeneric get_next_non_neutral_token()
        {
            CLua::TokenGeneric token;

            do
            {
                token = lexer.process_next_token();

                if (token.token_type == CLua::TokenType::Error)
                {
                    emit_lexer_error_for_token(token);
                    continue;
                }

                if (token.token_type == CLua::TokenType::EndOfFile)
                {
                    has_reached_eof = true;
                    return token;
                }
            }
            while (token.token_type == CLua::TokenType::Comment ||
                token.token_type == CLua::TokenType::NewLine ||
                token.token_type == CLua::TokenType::Whitespace);
                
            return token;
        }

        public:

        ParserContext(Common::Source& source): 
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
            NodeHandle node_handle = node_manager.create_node<Node>(std::forward<Args>(args)...);

            PAssert(
                node_handle.node_tag == CLuaNodes::NodeHandleTag::Valid,
                "some kind of unexpected behaviour from the code. Most likely cause is memory corruption"
            );

            return node_handle;
        };

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>) 
        inline Node* get_node_pointer_from_handle(NodeHandle node_handle){
            PAssert(
                node_handle.node_tag == NodeHandleTag::Valid,
                "node handle must be valid to be casted from node handle to node pointer"
            )
            return node_manager.get_node_pointer_from_handle<Node>(node_handle);
        }

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>)
        inline Node& get_node_from_handle(NodeHandle node_handle)
        {
            PAssert(
                node_handle.node_tag == NodeHandleTag::Valid,
                "node handle must be valid for function to be able to return a reference of a node"
            )
            return *get_node_pointer_from_handle<Node>(node_handle);
        };

        inline bool has_reached_end()
        {
            return has_reached_eof;
        };

        inline CLua::Lexer& get_lexer()
        {
            return lexer;
        }

        inline CLua::TokenGeneric get_next_token()
        {
            auto next_token = get_next_non_neutral_token();
            last_token = current_token;
            current_token = next_token;
            return next_token;
        };

        inline CLua::TokenGeneric see_current_token()
        {
            PAssert(
                current_token.token_type != TokenType::None,
                "unexpected behaviour from the see_current_token function"
            );
            return current_token;
        };

        inline CLua::TokenGeneric get_last_token() {
            return last_token;
        };

        inline CLua::NumberHint get_current_number_hint()
        {
            return lexer.get_current_number_hint();
        };

        inline CLua::Error get_current_error()
        {
            return lexer.get_current_error();
        };

        inline CLua::SymbolKind get_current_symbol()
        {
            if (current_token.token_type != CLua::TokenType::Symbol) [[unlikely]]
            {
                return CLua::SymbolKind::Unknown;
            };
            return lexer.get_current_symbol();
        };

        inline CLua::Keyword get_current_keyword()
        {
            return lexer.get_current_keyword();
        };

        inline char get_current_char_value()
        {
            return lexer.get_current_char_value();
        };

        inline Common::f64 get_current_fraction()
        {
            return lexer.get_current_fraction();
        };

        inline unsigned long long get_current_integer()
        {
            return lexer.get_current_integer();
        };

        NodeHandle emit_error(ParserError& parser_error)
        {
            //due to error spans, then it means that
            //the current assertion no longer fits under the requirements.
            //it should keep track of error spans to monitor overlaps and report
            //if they happen in the code
            auto error_id = error_list.size();
            auto error_handle = NodeHandle(
                NodeHandleTag::Error,
                error_id
            );

            error_list.push_back(parser_error);

            return error_handle;
        };
    
        inline bool is_symbol(CLua::SymbolKind expected_symbol)
        {
            return current_token.token_type == TokenType::Symbol && get_current_symbol() == expected_symbol;
        };

        inline bool is_identifier()
        {
            return current_token.token_type == TokenType::Identifier && 
            get_current_keyword() == CLua::Keyword::Unknown;
        };

        inline bool is_literal()
        {
            switch (current_token.token_type)
            {
                case TokenType::Char: case TokenType::String: case TokenType::Numeric:
                    return true;
                default:
                    return false;
            };
        };

        ParserState record_cursor()
        { 
            auto parser_state = ParserState();
            parser_state.current_token = current_token;
            parser_state.last_token = last_token;
            parser_state.has_reached_eof = has_reached_eof;
            parser_state.lexer_state = lexer.record_cursor();
            return parser_state;
        };

        void set_cursor(ParserState parser_state)
        {
            current_token = parser_state.current_token;
            last_token = parser_state.last_token;
            has_reached_eof = parser_state.has_reached_eof;
            lexer.set_cursor(parser_state.lexer_state);
        };
    };

    class Parser{
        ParserContext parser_context;
        public:

        Parser(Common::Source& source): parser_context(source)
        {};        

        NodeHandle generate_AST();

        private:

        void print_node_tree(NodeHandle,Common::uint64);
        
        void print_error_node(NodeHandle node_handle,Common::uint64 current_depth)
        {
            std::cout << "Error Node ID: " << static_cast<const char*>(node_handle) << std::endl;
        };

        std::string_view get_token_text(const CLua::TokenGeneric& token)
        {
            auto& source = parser_context.get_lexer().get_lexer_context().source;
            return source.slice_string(token.offset, token.length);
        }

        void print_valid_node(NodeHandle node_handle,Common::uint64 current_depth)
        {
            using namespace CLuaNodes;
            auto* node_ptr = parser_context.get_node_pointer_from_handle<BaseNode>(node_handle);
            auto node_type = node_ptr->node_type;
            std::string indent(current_depth * 2, ' ');
            current_depth++;
            
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
                std::cout << indent << "IdentifierPath: " << std::endl;
                print_node_tree(identifier_path_node->identifier, current_depth);


                auto next_segment_handle = identifier_path_node->next_segment;

                if(next_segment_handle.node_tag != NodeHandleTag::Valid)
                {
                    return;
                };
                print_node_tree(next_segment_handle,current_depth);
                break;
            }
            case NodeType::GroupExpression: {
                std::cout << indent << "GroupExpression:" << std::endl;

                auto group_expression = static_cast<CLuaNodes::GroupExpression*>(node_ptr);
                print_node_tree(group_expression->group_expression,current_depth);

                break;
            }
            case NodeType::Action: {
                std::cout << indent << "Action" << std::endl;
                auto* action_node = static_cast<CLuaNodes::ActionNode*>(node_ptr);
                print_node_tree(action_node->action_description, current_depth + 1);
                print_node_tree(action_node->next_action, current_depth + 1);
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