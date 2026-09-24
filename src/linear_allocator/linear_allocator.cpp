#include "linear_allocator.hpp"
#include <memory>

namespace Common {
    static uint64 align(const uint64 size, const uint64 top_position)
    {
        const uint64 alignment = std::bit_ceil(size);

        const uint64 mask = alignment - 1;
        const uint64 aligned_address = (top_position + mask) & ~mask;
        
        return aligned_address;
    };

    uint64 LinearAllocator::allocate(const uint64 size)
    {
        const auto new_element_address = align(size,memory_top);
        const auto new_top = new_element_address + size;
        memory_top = new_top;

        if (memory_region_size < new_top)
        {
            const auto new_size = static_cast<uint64>(static_cast<double>(new_top) * 1.5);

            const auto new_region = new Byte[new_size];

            memcpy(new_region,memory_region_start,memory_region_size);

            delete[] memory_region_start;
            
            memory_region_start = new_region;
            memory_region_size = new_size;
        };

        auto current_index = new_element_address;
        return current_index;
    };
};