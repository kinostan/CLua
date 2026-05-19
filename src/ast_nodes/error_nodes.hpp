#pragma once

#include <common/base.hpp>
#include <common/clua/tokens.hpp>

#include "base.hpp"

namespace CLuaNodes {
    class UnclosedBlockError : public BaseErrorNode {
        public:
        UnclosedBlockError(CLua::TokenSpan& token_span): BaseErrorNode(token_span)
        {
            node_type = NodeType::UnclosedBlockError;
        };
    };

    class UnclosedGroupError : public BaseErrorNode {
        public:
        UnclosedGroupError(CLua::TokenSpan& token_span): BaseErrorNode(token_span)
        {
            node_type = NodeType::UnclosedGroupError;
        };
    };

    class UnexpectedEOFError : public BaseNode {
        public:
        CLua::TokenGeneric token;

        UnexpectedEOFError()
        {
            node_type = NodeType::UnexpectedEOFError;
        };
    };

    class LexerErrorNode : public BaseNode {
        public:
        CLua::TokenGeneric token;
        CLua::Error error_code = CLua::Error::None;

        LexerErrorNode()
        {
            node_type = NodeType::LexerError;
        };

        LexerErrorNode(CLua::TokenGeneric token, CLua::Error error_code):
            token(token), error_code(error_code)
        {
            node_type = NodeType::LexerError;
        };
    };

    class UnexpectedTokenError: public BaseNode {
        public:
        CLua::TokenGeneric token;

        UnexpectedTokenError(CLua::TokenGeneric token): token(token)
        {
            node_type = NodeType::UnexpectedTokenError;
        };
    };

    class MissingTokenError : public BaseNode {
        public:
        CLua::TokenGeneric token;
        CLua::TokenType expected_token_type = CLua::TokenType::None;

        MissingTokenError()
        {
            node_type = NodeType::MissingTokenError;
        };
    };

    class InvalidIdentifierError : public BaseNode {
        public:
        CLua::TokenGeneric token;

        InvalidIdentifierError()
        {
            node_type = NodeType::InvalidIdentifierError;
        };
    };

    class InvalidExpressionError : public BaseNode {
        public:
        CLua::TokenGeneric token;

        InvalidExpressionError()
        {
            node_type = NodeType::InvalidExpressionError;
        };
    };

    class IdentifierExpected : public BaseNode {
        public: 
        CLua::TokenGeneric got_token;

        IdentifierExpected(CLua::TokenGeneric got_token): got_token(got_token)
        {
            node_type = NodeType::IdentifierExpectedError;
        };
    };
}
