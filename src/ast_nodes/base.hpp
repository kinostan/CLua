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

    enum class NodeHandleTag : Common::uint64 {
        Valid     = 0b00ULL << 62, 
        Reserved  = 0b01ULL << 62, 
        NoPattern = 0b10ULL << 62,
        Error     = 0b11ULL << 62  
    };

    using NodeHandle = Common::uint64;

    const NodeHandle NodeTagMask = 0b11ULL << 62;
    const NodeHandle InvalidNode = ULLONG_MAX;

    inline NodeHandleTag get_node_tag_from_handle(NodeHandle node_handle)
    {
        PAssert(
            static_cast<NodeHandleTag>(node_handle & NodeTagMask) != NodeHandleTag::Reserved,
            "unexpected behaviour, invalid state of the NodeHandleTag property type (Reserved) is set, possible memory" 
            " corruption"
        );

        return static_cast<NodeHandleTag>(node_handle & NodeTagMask);
    };

    inline NodeHandle set_node_state_for_handle(NodeHandle node_handle,NodeHandleTag node_state){
        return static_cast<NodeHandle>(node_state) | (node_handle & ~NodeTagMask); 
    };  

    inline Common::uint64 get_error_id_from_node_handle(NodeHandle node_handle)
    {
        auto node_state = get_node_tag_from_handle(node_handle);
        Assert(
            node_state == NodeHandleTag::Error,
            "get_error_id_from_node_handle called when node_handle is not an error node"
        );
        return node_handle & ~NodeTagMask;
    };
    
    inline NodeHandle create_error_node_handle(Common::uint64 error_id)
    {
        Assert(
            (error_id & ~NodeTagMask) == error_id,
            "Error id exceeded the maximum number range"
        );
        return static_cast<NodeHandle>(NodeHandleTag::Error) | (error_id & ~NodeTagMask);
    };

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
        long double value = 0;
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
        IdentifierPathNode(CLua::TokenGeneric identifier_token,bool has_scope_symbol):
        identifier_token(identifier_token), has_scope_symbol(has_scope_symbol)
        {
            node_type = NodeType::IdentifierPath;
        }

        bool has_scope_symbol;
        CLua::TokenGeneric identifier_token;
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
