#pragma once

#include <common/base.hpp>

#include <common/language_processing/node_manager.hpp>
#include <common/language_processing/node_handle.hpp>
#include <common/language_processing/lexer.hpp>

#include <iostream>
#include <vector>

namespace Util::Parser {

    using NodeHandle = AST::NodeHandle;
    using BaseNode = AST::BaseNode;

    using TokenGeneric = Util::Lexer::TokenGeneric;
    using TokenSpan = Util::Lexer::TokenSpan;
    using TokenType = Util::Lexer::TokenType;

    struct ParserState {
        bool has_reached_eof = false;     
        TokenGeneric current_token; 
        Util::Lexer::LexerState lexer_state;
    };

    class ParserContext {
    private:
        bool has_reached_eof = false;

        Util::Lexer::Lexer lexer;
        AST::NodeManager node_manager;
        
        TokenGeneric current_token;
        std::vector<NodeHandle> error_list;

    public:
        ParserContext(Common::Source& source) : 
            lexer(source), 
            node_manager(source.source_size / 3),
            current_token{}
        {};

        template<typename Node, typename... Args>
        requires (std::derived_from<Node, BaseNode> && std::is_constructible_v<Node, Args...>)
        inline NodeHandle create_node(Args&&... args)
        {
            NodeHandle node_handle = node_manager.create_node<Node>(std::forward<Args>(args)...);
            PAssert(node_handle.is_valid(), "Node allocations hit structural corruption thresholds.");
            return node_handle;
        };

        template<typename Node>
        requires (std::derived_from<Node, BaseNode>) 
        inline Node* get_node_pointer_from_handle(NodeHandle node_handle) {
            return node_manager.get_node_pointer_from_handle<Node>(node_handle);
        }

        inline bool has_reached_end() const { return has_reached_eof; };
        inline Util::Lexer::Lexer& get_lexer() { return lexer; }

        inline void advance_token()
        {
            PAssert(
                current_token.offset >= lexer.get_lexer_context().source.index 
                && current_token.token_type != TokenType::None,
                "assertion failed as the parser tried to advance token witout seeing that next token first"
            );
            lexer.commit_token_window(current_token);
        };

        inline TokenGeneric peek_next_token()
        {
            current_token = lexer.peek_next_token();
            return current_token;
        };

        inline TokenGeneric see_current_token()
        {
            PAssert(
                current_token.token_type != TokenType::None,
                "assertion failed because token_type of current_token is invalid (token_type == None)"
            );          
            return current_token;
        };

        inline std::string_view get_token_text(TokenSpan& span)
        {
            return lexer.get_lexer_context().source.slice_string(span);
        }

        ParserState record_cursor()
        { 
            ParserState parser_state{};
            parser_state.current_token = current_token;
            parser_state.has_reached_eof = has_reached_eof;
            parser_state.lexer_state = lexer.record_cursor();
            return parser_state;
        };

        void set_cursor(ParserState parser_state)
        {
            current_token = parser_state.current_token;
            has_reached_eof = parser_state.has_reached_eof;
            lexer.set_cursor(parser_state.lexer_state);
        };

        NodeHandle emit_error(const NodeHandle& error_node) { 
            auto error_id = error_list.size();
            auto error_handle = NodeHandle(
                AST::NodeHandleTag::Error,
                error_id
            );


            error_list.push_back(error_node); 
            return error_handle;
        };

        private:
        size_t record_error_state() { 
            return error_list.size(); 
        }

        void set_error_state(size_t target_error_state) { 
            error_list.resize(target_error_state); 
        };
    };

    class IParser {
    public:
        virtual ~IParser() = default;
        virtual NodeHandle generate_AST() = 0;
    };
}