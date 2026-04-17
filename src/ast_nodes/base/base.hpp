#pragma once

#include <lexer/lexer.hpp>

namespace CLuaNodes {
    enum class NodeType {
        Invalid,
        CharLiteral,
        StringLiteral,
        NumberLiteral,
        IntegerLiteral,

        Identifier,
        IdentifierPath,

        Action,
        Expression,

        UnaryExpression,
        BinaryExpression,
        TernaryExpression
    };

    enum class IdentifierPathSeparator {
        None,
        Dot,
        DoubleColon
    };

    enum class UnOperationType{
        None,
        Minus,
        
        Reference,
        Dereference,

        PreIncrement,
        PreDecrement,
        PostIncrement,
        PostDecrement,
    };

    enum class BinOperationType {
        None,
        Assign,

        Add,
        Sub,

        Mul,
        Div,
        Mod,

        //comparative
        Eq, //equal
        Gt, //greater than
        Lt, //less than
        GtEq, //greater or equal
        LtEq, //less or equal

        //logical
        Or,
        And,

        //bitwise operations
        BinOr,
        BinAnd,
        BinXor,

        BinRight,
        BinLeft
    };

    enum class TernarnyOperationType{
        None,
        Conditional
    };

    using NodeHandle = size_t;

    const NodeHandle InvalidNodeMask = ~LLONG_MAX;
    const NodeHandle InvalidNode = ULLONG_MAX;

    class BaseNode {
        public:
        NodeType node_type = NodeType::Invalid;
    };

    class CharLiteral: public BaseNode {
        char value;
    };

    class StringLiteral: public BaseNode {
        
        char* string;
    };

    class NumberNode : public BaseNode {
        public:
        NumberNode()
        {
            node_type = NodeType::NumberLiteral;
        };
        long double value = 0;
    };

    class IntegerLiteral: public BaseNode {
        public:
        IntegerLiteral()
        {
            node_type = NodeType::IntegerLiteral;
        };
        long long value = 0;
    };


    class Identifier: public BaseNode {
        public:
        Identifier()
        {
            node_type = NodeType::Identifier;
        };
        Util::TokenGeneric identifier_token;
    };

    class IdentifierPathNode : public BaseNode {
    public:
        IdentifierPathNode() {
            node_type = NodeType::IdentifierPath;
        }
        Util::TokenGeneric identifier_token;
        IdentifierPathSeparator separator_from_previous = IdentifierPathSeparator::None;
        NodeHandle next_segment = InvalidNode;
    };

    class TernaryNode : public BaseNode {
        public:
        TernaryNode()
        {
            node_type = NodeType::TernaryExpression;
        };
        NodeHandle left = InvalidNode;
        NodeHandle center = InvalidNode;
        NodeHandle right = InvalidNode;
    };

    class BinaryNode : public BaseNode {
        public:
        BinaryNode(BinOperationType operation_type): operation_type(operation_type)
        {
            node_type = NodeType::BinaryExpression;
        };
        NodeHandle left = InvalidNode;
        NodeHandle right = InvalidNode;
        BinOperationType operation_type = BinOperationType::None;
    };

    class UnaryNode : public BaseNode {
        public:
        UnaryNode(UnOperationType operation_type): operation_type(operation_type)
        {
            node_type = NodeType::UnaryExpression;
        };
        NodeHandle node = InvalidNode;
        UnOperationType operation_type = UnOperationType::None;
    };

    class ExpressionNode : public BaseNode {
        public:
        ExpressionNode(NodeHandle expression_node_handle): expression_node_handle(expression_node_handle)
        {
            node_type = NodeType::Expression;
        };
        NodeHandle expression_node_handle;
    };

    class ActionNode : public BaseNode {
        public:
        ActionNode(NodeHandle action_description): action_description(action_description)
        {
            node_type = NodeType::Action;
        };
        NodeHandle action_description;
        NodeHandle next_action;
    };
}
