#pragma once

#include <common/language_processing/parser.hpp>
#include <common/language_processing/node_handle.hpp>
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
            AST::NodeHandle identifier;
        };

        struct FuncCallNode: AST::BaseNode {
            AST::NodeHandle identifier;
        };

        struct Comment: AST::BaseNode {
            Common::TokenSpan token_span;
        };
    }

    enum class Symbol {
        If,
        Unknown
    };

    enum class ErrorCode {
        Unspecified,
        ExpectedKeyword,
        ExpectedIdentifier,
        UnknownStatement,
        ExpectedCloseParen,
        ExpectedOpenParen,
        ExpectedWhitespace,
        ExpectedCommentStart,
        ExpectedCommentEnd,
        ExpectedNewline,
        ExpectedStringClosure
    };

    const Common::uint64 LanguageID = 0;
    class Parser: public Common::IParser {
       AST::NodeHandle generate_AST(Common::ParserContext& parser_context) override;
    };
}