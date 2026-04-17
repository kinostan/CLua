#include "linear_allocator.hpp"
#include <cstdlib>
#include <memory>

namespace Util {
    size_t align(size_t size, size_t top_position)
    {
        size_t alignment = std::bit_ceil(size);
        
        size_t mask = alignment - 1;
        size_t aligned_address = (top_position + mask) & ~mask;
        
        return aligned_address;
    };

    size_t LinearAllocator::allocate(size_t size)
    {
        auto new_element_address = align(size,memory_top);
        auto new_top = new_element_address + size;
        memory_top = new_top;

        if (memory_region_size < new_top)
        {
            auto new_size = static_cast<size_t>(new_top * 1.5);

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