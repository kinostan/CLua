#include "linear_allocator.hpp"
#include <cstdlib>
#include <memory>

namespace Common {
    Common::uint64 align(Common::uint64 size, Common::uint64 top_position)
    {
        Common::uint64 alignment = std::bit_ceil(size);
        
        Common::uint64 mask = alignment - 1;
        Common::uint64 aligned_address = (top_position + mask) & ~mask;
        
        return aligned_address;
    };

    Common::uint64 LinearAllocator::allocate(Common::uint64 size)
    {
        auto new_element_address = align(size,memory_top);
        auto new_top = new_element_address + size;
        memory_top = new_top;

        if (memory_region_size < new_top)
        {
            auto new_size = static_cast<Common::uint64>(new_top * 1.5);

            auto new_region = new Byte[new_size];

            memcpy(new_region,memory_region_start,memory_region_size);

            delete[] memory_region_start;
            
            memory_region_start = new_region;
            memory_region_size = new_size;
        };

        auto current_index = new_element_address;
        return current_index;
    };
};