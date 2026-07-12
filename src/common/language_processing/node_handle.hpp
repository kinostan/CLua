#pragma once

#include <common/base.hpp>
#include <debugger/debugger.hpp>

namespace AST { 
    enum class NodeHandleTag : Common::uint8 {
        Valid         = 0b00,   //NodeHandle defined by a user
        CompilerData  = 0b01,   //TokenSpan or LinkedNode list
        Error         = 0b10,   //Explicit null return
        CommitedError = 0b11    //Error code defined by user
    };

    struct NodeHandle {
        NodeHandleTag node_tag: 2 = NodeHandleTag::Error;
        Common::uint64 node_value: 62 = 0;

        NodeHandle() = default;

        NodeHandle(NodeHandleTag node_tag, Common::uint64 node_value)
            : node_tag(node_tag), node_value(node_value)
        {
            Assert(
                static_cast<unsigned int>(node_tag) < 4 && node_value <= (ULLONG_MAX >> 2) - 1,
                "the field sizes can't be exceeded or sth" 
            );
        }

        NodeHandle& operator=(const NodeHandle& node) = default;
        
        bool operator==(const NodeHandle& node) const noexcept
        {
            typedef const Common::uint64 __attribute__((__may_alias__)) *safe_uint64_ptr;
            return *reinterpret_cast<safe_uint64_ptr>(this) == *reinterpret_cast<safe_uint64_ptr>(&node);
        }

        inline bool is_error() const
        {
            return node_tag == NodeHandleTag::Error || node_tag == NodeHandleTag::CommitedError;
        }

        inline bool is_commited_error() const
        {
            return node_tag == NodeHandleTag::CommitedError;
        }

        inline NodeHandle& commit()
        {
            Assert(
                is_error(),
                "Can't commit when node is not an error type"
            );
        
            node_tag = NodeHandleTag::CommitedError;

            return *this;
        };
    };
};