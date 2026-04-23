#pragma once

#include <base.hpp>

#include <cstdint>

namespace Util {
    using Byte = unsigned char;

    static_assert(
        sizeof(Byte*) == sizeof(Util::uint64),
        "Size of a pointer and Util::uint64 differ, it's now unsafe to handle such case for linear allocator"
    );

    class LinearAllocator {
        public:        
        Byte* memory_region_start = nullptr;
        Util::uint64 memory_region_size = 0;
        Util::uint64 memory_top = 0;

        LinearAllocator() = default;
        LinearAllocator(Util::uint64 initial_size)
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
        
        Util::uint64 allocate(Util::uint64 object_count);
    };
};

