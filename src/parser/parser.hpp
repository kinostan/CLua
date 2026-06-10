#pragma once

#include <common/base.hpp>
#include "node_manager.hpp"

#include <clua_lexer/lexer.hpp>

#include <ast_nodes/base.hpp>
#include <ast_nodes/error_nodes.hpp>

#include <iostream>
#include <vector>

/*
 If I ever want to expand the scope of the target emittion language from just CLua to 
 CLua and LuaU I need to add a separate emitter for parser.hpp which implies that 
 tokens should be universalized in some way so that parser can use them. 
*/
namespace CLuaASTParser{

    using NodeType = CLuaNodes::NodeType;
    using NodeHandleTag = CLuaNodes::NodeHandleTag;
    using NodeHandle = CLuaNodes::NodeHandle;
    using BaseNode = CLuaNodes::BaseNode;

    using TokenGeneric = CLua::TokenGeneric;
    using TokenSpan = CLua::TokenSpan;
    using TokenType = CLua::TokenType;
    
    const auto InvalidNode = CLuaNodes::InvalidNode;


    enum Error {
        None
    };
    
    //AST-concept-1 should only parse math expressions and evaluate them immedieatly i
    class ParserError {
        public:

        TokenSpan token_span;
        CLua::Error lexer_error = CLua::Error::None;
        Error parser_error = Error::None;

        ParserError(TokenGeneric start, TokenGeneric end): token_span(TokenSpan(start,end))
        {};
        ParserError(TokenGeneric only_token): token_span(TokenSpan(only_token,only_token))
        {};

        void attach_lexer_error(CLua::Error lexer_error)
        {
            this->lexer_error = lexer_error;
        };

        void attach_parser_error(Error parser_error)
        {
            this->parser_error = parser_error;
        };
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

        inline Common::uint64 get_current_integer()
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

        size_t record_error_state()
        {
            return error_list.size();
        }

        void set_error_state(size_t target_error_state) 
        {
            error_list.resize(target_error_state);
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
    };

    class Parser{
        ParserContext parser_context;
        public:

        Parser(Common::Source& source): parser_context(source)
        {};        

        NodeHandle generate_AST();

        private:

        void print_node_tree(NodeHandle,Common::uint64);
        
        void print_error_node(NodeHandle node_handle,Common::uint64 current_depth);

        std::string_view get_token_text(const CLua::TokenGeneric& token)
        {
            auto& source = parser_context.get_lexer().get_lexer_context().source;
            return source.slice_string(token.offset, token.length);
        }
        
        public:
        void print_node_tree(NodeHandle node_handle) {
            return print_node_tree(node_handle,0);
        };
    };
}