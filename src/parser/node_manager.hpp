#pragma once

#include <common/base.hpp>

#include <concepts>
#include <type_traits>

#include <linear_allocator/linear_allocator.hpp>
#include <ast_nodes/base.hpp>

namespace CLuaNodes{ 
    class NodeManager {
        using NodeHandle = NodeHandle;
        using BaseNode = BaseNode;
        private:
        Common::LinearAllocator allocator;
        public:
        NodeManager(Common::uint64 initial_allocated_memory_region): 
        allocator(Common::LinearAllocator(initial_allocated_memory_region))
        {};

        template<typename Node, typename... Args>
        requires (std::derived_from<Node,BaseNode>) && std::is_constructible_v<Node, Args...>
        inline NodeHandle create_node(Args&&... args)
        {
            auto offset = allocator.allocate(sizeof(Node));
            auto node = NodeHandle(NodeHandleTag::Valid,offset);
            new (get_node_pointer_from_handle<Node>(node)) Node(std::forward<Args>(args)...);
            return node;
        };

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>)
        inline Node& get_node_from_handle(NodeHandle node_handle)
        {
            Assert(
                node_handle.node_tag == NodeHandleTag::Valid,
                "in order to get a reference of a node, the node handle must be valid"
            )
            return *(reinterpret_cast<Node*>(allocator.memory_region_start + node_handle.node_value));
        };

        template<typename Node, typename... Args>
        requires (std::derived_from<Node,BaseNode>) 
        inline Node* get_node_pointer_from_handle(NodeHandle node_handle)
        {
            Assert(
                node_handle.node_tag == NodeHandleTag::Valid,
                "in order to get a pointer of a node, the node handle must be valid"
            )
            return reinterpret_cast<Node*>(allocator.memory_region_start + node_handle.node_value);
        };
    };
};