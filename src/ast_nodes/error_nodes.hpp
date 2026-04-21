#pragma once
#include "base/base.hpp"

namespace CLuaNodes {
    class UnclosedBlockError : public BaseNode {
        public:
        Util::TokenGeneric opening_token;

        UnclosedBlockError()
        {
            node_type = NodeType::UnclosedBlockError;
        };
    };

    class UnclosedGroupError : public BaseNode {
        public:
        Util::TokenGeneric opening_token;

        UnclosedGroupError()
        {
            node_type = NodeType::UnclosedGroupError;
        };
    };

    class UnexpectedEOFError : public BaseNode {
        public:
        Util::TokenGeneric token;

        UnexpectedEOFError()
        {
            node_type = NodeType::UnexpectedEOFError;
        };
    };

    class LexerErrorNode : public BaseNode {
        public:
        Util::TokenGeneric token;
        Util::ErrorCode error_code = Util::ErrorCode::None;

        LexerErrorNode()
        {
            node_type = NodeType::LexerError;
        };

        LexerErrorNode(Util::TokenGeneric token, Util::ErrorCode error_code):
            token(token), error_code(error_code)
        {
            node_type = NodeType::LexerError;
        };
    };

    class UnexpectedTokenError: public BaseNode {
        public:
        Util::TokenGeneric token;

        UnexpectedTokenError(Util::TokenGeneric token): token(token)
        {
            node_type = NodeType::UnexpectedTokenError;
        };
    };

    class MissingTokenError : public BaseNode {
        public:
        Util::TokenGeneric token;
        Util::TokenType expected_token_type = Util::TokenType::None;

        MissingTokenError()
        {
            node_type = NodeType::MissingTokenError;
        };
    };

    class InvalidIdentifierError : public BaseNode {
        public:
        Util::TokenGeneric token;

        InvalidIdentifierError()
        {
            node_type = NodeType::InvalidIdentifierError;
        };
    };

    class InvalidExpressionError : public BaseNode {
        public:
        Util::TokenGeneric token;

        InvalidExpressionError()
        {
            node_type = NodeType::InvalidExpressionError;
        };
    };

    class IdentifierExpected : public BaseNode {
        public: 
        Util::TokenGeneric got_token;

        IdentifierExpected(Util::TokenGeneric got_token): got_token(got_token)
        {
            node_type = NodeType::IdentifierExpectedError;
        };
    };
}
