#pragma once

#include <common/language_processing/base.hpp>
#include <common/base.hpp>

#include <common/language_processing/node_handle.hpp>

#include <debugger/debugger.hpp>

namespace AST {
    using NodeType = Common::uint16;

    enum struct BaseTypes {
        Invalid,
        LinkedList,
        TokenSpan
    };

    NodeType Invalid = 0;
    NodeType UErrorNode = Invalid;

    using ErrorCode = Common::uint16;
    
    namespace ErrorCodes {
        ErrorCode None = 0;
    };

    struct BaseNode {
        public:
        NodeType node_type = Invalid;
    };

    //node_type = ... is not going to make an effect during initialization
    //mainly because of how nodes are being created now
    //that means that .node_type is not assigned automatically and it's only
    //used here as a visual information for the user
    struct LinkedNodeList: public BaseNode{
        NodeType node_type = static_cast<NodeType>(BaseTypes::LinkedList);
        NodeHandle value;
        NodeHandle next;
    };

    struct TokenSpanNode: public BaseNode{
        NodeType node_type = static_cast<NodeType>(BaseTypes::TokenSpan);
        Common::TokenSpan token_span;
    };

    struct BaseErrorNode: public BaseNode {
        Common::TokenSpan error_span;
        ErrorCode error_code = ErrorCodes::None;
    };
}
