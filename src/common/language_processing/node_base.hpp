#pragma once

#include <common/base.hpp>

#include <common/language_processing/node_handle.hpp>
#include <common/language_processing/tokens.hpp>

#include <debugger/debugger.hpp>

namespace AST {
    using NodeType = Common::uint16;

    NodeType Invalid = 0;
    NodeType UErrorNode = Invalid;

    using ErrorCode = Common::uint16;
    
    namespace ErrorCodes {
        ErrorCode None = 0;
    };

    class BaseNode {
        public:
        NodeType node_type = Invalid;
    };

    class BaseErrorNode: public BaseNode {
        Util::Lexer::TokenSpan error_span;
        ErrorCode error_code = ErrorCodes::None;

        BaseErrorNode(Util::Lexer::TokenSpan error_span): error_span(error_span)
        {
            node_type = UErrorNode;
        };
    };
}
