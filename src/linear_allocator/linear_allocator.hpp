#pragma once
#include <cstdint>

namespace Util {
    using Byte = unsigned char;

    static_assert(
        sizeof(Byte*) == sizeof(size_t),
        "Size of a pointer and size_t differ, it's now unsafe to handle such case for linear allocator"
    );

    class LinearAllocator {
        public:        
        Byte* memory_region_start = nullptr;
        size_t memory_region_size = 0;
        size_t memory_top = 0;

        LinearAllocator() = default;
        LinearAllocator(size_t initial_size)
        {
            memory_region_start = new Byte[initial_size];
            memory_top = 0;
            if (memory_region_start == nullptr)
            {
                memory_region_size = 0;
                return;
            };
            memory_region_size = initial_size;
        };  

        ~LinearAllocator()
        {
            delete[] memory_region_start;
            memory_region_size = 0;
        };
        
        size_t allocate(size_t object_count);
    };
};

