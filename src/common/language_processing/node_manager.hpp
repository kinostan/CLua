#pragma once

#include <common/base.hpp>

#include <concepts>

#include <linear_allocator/linear_allocator.hpp>
#include <common/language_processing/node_handle.hpp>
#include <common/language_processing/node_base.hpp>

namespace AST{ 
    class NodeManager {
        using NodeHandle = NodeHandle;
        Common::LinearAllocator allocator;
        public:
        NodeManager(Common::uint64 initial_allocated_memory_region): 
        allocator(Common::LinearAllocator(initial_allocated_memory_region))
        {};

        template<typename Node>
        
        [[nodiscard]] NodeHandle create_node()
        {
            auto offset = allocator.allocate(sizeof(Node));
            auto node = NodeHandle(NodeHandleTag::Valid,offset);
            new (get_node_pointer_from_handle<Node>(node)) Node();
            return node;
        };

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>)
        [[nodiscard]] Node& get_node_from_handle(const NodeHandle node_handle)
        {
            Assert(
                node_handle.node_tag == NodeHandleTag::Valid,
                "in order to get a reference of a node, the node handle must be valid"
            )
            return *(reinterpret_cast<Node*>(allocator.memory_region_start + node_handle.node_value));
        };

        template<typename Node>
        requires (std::derived_from<Node,BaseNode>)
        [[nodiscard]] Node* get_node_pointer_from_handle(const NodeHandle node_handle)
        {
            Assert(
                node_handle.node_tag == NodeHandleTag::Valid,
                "in order to get a pointer of a node, the node handle must be valid"
            )
            return reinterpret_cast<Node*>(allocator.memory_region_start + node_handle.node_value);
        };

        void set_linear_memory_offset(const Common::uint64 new_offset)
        {
            allocator.memory_top = new_offset;
        };

        [[nodiscard]] Common::uint64 get_linear_memory_offset() const
        {
            return allocator.memory_top;
        };
    };
};