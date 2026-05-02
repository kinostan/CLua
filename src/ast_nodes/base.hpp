#pragma once

#include <common/base.hpp>
#include <common/clua/tokens.hpp>

#include <debugger/debugger.hpp>

namespace CLuaNodes {
    enum class NodeType {
        Invalid,
        CharLiteral,
        StringLiteral,
        NumberLiteral,
        IntegerLiteral,

        Identifier,
        IdentifierPath,

        GroupExpression,

        Action,
        Expression,

        UnaryExpression,
        BinaryExpression,
        TernaryExpression,

        //Error Node Type
        UnclosedBlockError,
        UnclosedGroupError,
        LexerError,
        UnexpectedEOFError,
        UnexpectedTokenError,
        MissingTokenError,
        InvalidIdentifierError,
        InvalidExpressionError,
        IdentifierExpectedError
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

    enum class NodeHandleTag : Common::uint8 {
        Valid     = 0b00, 
        Reserved  = 0b01, 
        NoPattern = 0b10,
        Error     = 0b11  
    };

    struct NodeHandle {
        NodeHandleTag node_tag: 2 = NodeHandleTag::Error;
        Common::uint64 node_value: 62 = 0;

        NodeHandle(NodeHandle& node_handle) = default;
        NodeHandle(const NodeHandle& node_handle) = default;
        NodeHandle() = default; 

        NodeHandle(NodeHandleTag node_tag,Common::uint64 node_value): node_tag(node_tag), node_value(node_value)
        {
            Assert(
                static_cast<unsigned int>(node_tag) < 4 && node_value <= (ULLONG_MAX >> 2) - 1,
                "the field sizes can't be exceeded or sth" 
            );
            Assert(
                node_tag != NodeHandleTag::Reserved,
                "invalid stat3e for node tag is being set"
            );

        };

        NodeHandle& operator=(NodeHandle& node) = default;
        NodeHandle& operator=(const NodeHandle& node) = default;

        operator const char*()
        {
            return "Node Handle Instance";
        };
    };

    const NodeHandle InvalidNode = NodeHandle(
        NodeHandleTag::Error,
        (ULLONG_MAX >> 2) - 1
    );

    const NodeHandle NoPatternNode = NodeHandle(    
        NodeHandleTag::NoPattern,
        0
    );

    class BaseNode {
        public:
        NodeType node_type = NodeType::Invalid;
    };

    class CharLiteral: public BaseNode {
        public:
        CharLiteral(char char_value): value(char_value) {
            node_type = NodeType::CharLiteral;
        };
        char value;
    };

    class StringLiteral: public BaseNode {
        public:
        StringLiteral(CLua::StringToken string_token): string_token(string_token)
        {
            node_type = NodeType::StringLiteral;
        };
        CLua::StringToken string_token;
    };

    class NumberNode : public BaseNode {
        public:
        NumberNode()
        {
            node_type = NodeType::NumberLiteral;
        };
        Common::f64 value = 0;
    };

    class IntegerLiteral: public BaseNode {
        public:
        IntegerLiteral(Common::uint64 value): value(value)
        {
            node_type = NodeType::IntegerLiteral;
        };
        Common::uint64 value = 0;
    };


    class Identifier: public BaseNode {
        public:
        Identifier(CLua::IdentifierToken identifier_token): identifier_token(identifier_token)
        {
            node_type = NodeType::Identifier;
        };
        CLua::TokenGeneric identifier_token;
    };

    class IdentifierPathNode : public BaseNode {
    public:
        IdentifierPathNode(NodeHandle identifier,bool has_scope_symbol):
        identifier(identifier), has_scope_symbol(has_scope_symbol)
        {
            node_type = NodeType::IdentifierPath;
        }

        bool has_scope_symbol;
        NodeHandle identifier;
        NodeHandle next_segment = InvalidNode;
    };

    class GroupExpression : public BaseNode {
        public:
        GroupExpression(NodeHandle group_expression): group_expression(group_expression)
        {
            node_type = NodeType::GroupExpression;
        };
        NodeHandle group_expression = InvalidNode;
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
        NodeHandle expression_node_handle = InvalidNode;
    };

    class ActionNode : public BaseNode {
        public:
        ActionNode(NodeHandle action_description): action_description(action_description)
        {
            node_type = NodeType::Action;
        };
        NodeHandle action_description = InvalidNode;
        NodeHandle next_action = InvalidNode;
    };
}
