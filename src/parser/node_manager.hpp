#pragma once

#include <concepts>
#include <type_traits>

#include <linear_allocator/linear_allocator.hpp>
#include <ast_nodes/base/base.hpp>

namespace Util{ 
    class NodeManager {
        using NodeHandle = CLuaNodes::NodeHandle;
        using BaseNode = CLuaNodes::BaseNode;
        private:
        Util::LinearAllocator allocator;
        public:
        NodeManager(size_t initial_allocated_memory_region): 
        allocator(Util::LinearAllocator(initial_allocated_memory_region))
        {};

        template<typename Node, typename... Args>
        requires (std::derived_from<Node,BaseNode>) && std::is_constructible_v<Node, Args...>
        inline NodeHandle create_node(Args&&... args)
        {
            auto node_handle = allocator.allocate(sizeof(Node));
            new (get_node_pointer_from_handle<Node>(node_handle)) Node(std::forward<Args>(args)...);
            return node_handle;
        };

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>)
        inline Node& get_node_from_handle(NodeHandle node_handle)
        {
            return *(static_cast<Node*>(allocator.memory_region_start + node_handle));
        };

        template<typename Node, typename... Args>
        requires (std::derived_from<Node,BaseNode>) 
        inline Node* get_node_pointer_from_handle(NodeHandle node_handle)
        {
            return static_cast<Node*>(allocator.memory_region_start + node_handle);
        };
    };
};