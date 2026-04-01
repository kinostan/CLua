#include <linear_allocator/linear_allocator.hpp>
#pragma once

namespace CLuaNodes {
    enum class NodeType {
        Null,
        Unassgined,
        CharLiteral,
        StringLiteral,
        NumberLiteral,
        IntegerLiteral,
    };

    enum class OperationType {
        Add,
        Sub,
        Mul,
        Div
    };

    using NodeHandle = size_t;

    class NodeManager {
        private:
        Util::LinearAllocator& allocator;
        public:
        NodeManager(Util::LinearAllocator allocator): allocator(allocator)
        {}

        template<typename Node>
        Node* get_node(NodeHandle reference)
        {
            return static_cast<Node*>(allocator.memory_region_start + reference.offset);
        };

        template<typename Node>
        requires is_base_of<Node>;
        NodeHandle create_node(Node node_value)
        {
            NodeHandle ast_node_reference;
            ast_node_reference = allocator.allocate(sizeof(Node));
            *(static_cast<Node*>(allocator.memory_region_start)) = node_value;

            return ast_node_reference;
        };
    };

    class Identifier{
        char* identifier;      
    };

    class IdentifierPath {
        
    };

    struct Node {
        NodeType node_type;
    };

    struct NumberNode : Node {
        double value;
    };

    struct BinaryNode : Node {
        NodeHandle left;
        NodeHandle right;
        OperationType operation_type;
    };
};