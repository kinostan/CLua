#pragma once

#include <common/base.hpp>
#include <debugger/debugger.hpp>

namespace AST { 
    enum class NodeHandleTag : Common::uint8 {
        Valid     = 0b00,       //NodeHandle defined by a user
        CompilerData  = 0b01,   //TokenSpan or LinkedNode list
        NoPattern = 0b10,       //Explicit null return
        Error     = 0b11        //Error code defined by user
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
        };

        NodeHandle& operator=(NodeHandle& node) = default;
        NodeHandle& operator=(const NodeHandle& node) = default;

        operator const char*()
        {
            return "Node Handle Instance";
        };

        inline bool is_error()
        {
            return node_tag == NodeHandleTag::Error;
        };
    };
};