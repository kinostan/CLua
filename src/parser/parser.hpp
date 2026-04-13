#pragma once

#include "node_manager.hpp"

#include <lexer/lexer.hpp>
#include <ast_nodes/index.hpp>
#include <vector>

namespace ASTParser{
    //AST-concept-1 should only parse math expressions and evaluate them immedieatly i
    enum class ParserErrorCode {
        None,
        LexerError,
        InvalidExpression
    };

    class ParserError {
        public:
        ParserErrorCode error_code = ParserErrorCode::None;
        CLuaNodes::NodeHandle node_handle = InvalidNode;
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

        bool is_error_node(NodeHandle node_handle)
        {   
            return node_handle & InvalidNodeMask;
        };

        size_t get_error_id_from_node_handle(NodeHandle node_handle)
        {
            PAssert(
                !is_error_node(node_handle),
                "get_error_id_from_node_handle called when node_handle is a valid node"
            );

            return node_handle & ~InvalidNodeMask;
        };

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
        inline CLuaNodes::NodeHandle create_node(Args&&... args)
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

        Util::NumberHint get_last_number_hint()
        {
            return lexer.get_last_number_hint();
        };

        Util::Error get_last_error()
        {
            return lexer.get_last_error();
        };

        SymbolClassifier::SymbolKind get_current_symbol()
        {
            if (current_token.token_type != Util::TokenType::Symbol) [[unlikely]]
            {
                return SymbolClassifier::SymbolKind::Unknown;
            };
            return lexer.get_last_symbol();
        };

        KeywordClassifier::Keyword get_last_keyword()
        {
            return lexer.get_last_keyword();
        };
    
        //****Nodes****//
        CLuaNodes::NodeHandle create_error_handle(size_t error_id)
        {
            return InvalidNodeMask | error_id;
        };
        
        CLuaNodes::NodeHandle emit_error(ParserError& parser_error)
        {
            //due to error spans, then it means that
            //the current assertion no longer fits under the requirements.
            //it should keep track of error spans to monitor overlaps and report
            //if they happen in the code
            auto error_id = error_list.size();
            auto error_handle = create_error_handle(error_id);

            error_list.push_back(parser_error);

            return error_handle;
        };
    };

    class Parser{
        ParserContext parser_context;
        public:

        Parser(Util::Source& source): parser_context(source)
        {};        

        CLuaNodes::NodeHandle generate_AST();
    };
}