#pragma once

#include <common/language_processing/node_base.hpp>

namespace CLua {
    enum class NodeType {
        IdentifierNode,
        LocalDeclNode,
        FuncCallNode,
        Comment
    };

    namespace Nodes {
        struct IdentifierNode: AST::BaseNode{
            Common::TokenSpan span;
        };

        struct LocalDeclNode: AST::BaseNode {
            NodeHandle identifier;
        };

        struct FuncCallNode: AST::BaseNode {
            NodeHandle identifier;
        };

        struct Comment: AST::BaseNode {
            Common::TokenSpan token_span;
        };
    }
}