#pragma once

#include <common/language_processing/base.hpp>
#include <common/base.hpp>

#include <debugger/debugger.hpp>

#include <vector>
#include <type_traits>
#include <concepts>

namespace Common {
    struct SourceView {
        unsigned char* source_buffer;
        Common::uint64 source_size;
    };

    class Source {
        public:
        Common::uint64 peeked_char_index;
        Common::uint64 source_size;    
        private:   
        unsigned char* source_buffer;

        public:
            
        Source() = default;
        Source(unsigned char* source_buffer, Common::uint64 source_size) : 
        source_buffer(source_buffer), 
        source_size(source_size), 
        peeked_char_index(0)
        {
            Assert(source_buffer,
                "Source buffer must exist"
            );
        };

        std::string_view slice_string(Common::uint64 offset,Common::uint64 size)
        {   
            return std::string_view(
                reinterpret_cast<char*>(source_buffer + offset),
                size
            );
        };

        Source slice(Common::uint64 start_index,Common::uint64 length)
        {
            Assert(length > 0,
                "length must be greater than 0"
            )
            Common::uint64 end_index = start_index + length;
            Assert(
                end_index <= source_size,
                "broken assumption that end_index <= source_size is true"
            )
            return Source(source_buffer + start_index,length);
        };

        Source slice(Common::uint64 start_index)
        {
            Assert(
                source_size > start_index,
                "source_size > start_index is not true"
            )

            return slice(start_index,source_size-start_index);
        };

        inline unsigned char* get_source_buffer()
        {
            return source_buffer;
        };

        inline bool can_consume_sentinel(Common::uint64 consume_size = 1)
        {
            //source_size, because the additional character is a null terminator
            return peeked_char_index + (consume_size - 1) <= source_size;
        };

        inline bool can_consume(Common::uint64 consume_size = 1)
        {
            return peeked_char_index + (consume_size - 1) < source_size;
        };

        inline void consume(Common::uint64 consume_size = 1)
        {
            Assert(
                can_consume_sentinel(consume_size),
                "index is reading beyond the source_buffer"
            );

            peeked_char_index += consume_size;
        };

        inline unsigned char see_current()
        {
            Assert(
                can_consume_sentinel(),
                "index is reading beyond the source_buffer"
            );
            if (!can_consume())
            {
                return '\0';
            };
            return source_buffer[peeked_char_index];
        };

        inline bool can_peek_sentinel(Common::uint64 peek_offset = 0) const noexcept
        {  
            return (peeked_char_index+peek_offset) <= source_size;
        };

        inline bool can_peek(Common::uint64 peek_offset = 0) const noexcept
        {
                return (peeked_char_index+peek_offset) < source_size;
        };

        inline unsigned char peek(Common::uint64 peek_offset = 0)
        {   
            Assert(
                can_peek_sentinel(peek_offset),
                "Can't peek here"
            );
            if (!can_peek(peek_offset))
            {
                return (unsigned char)'\0';
            };
            return source_buffer[peeked_char_index+peek_offset];
        };
        
        inline void set_peek_index(Common::uint64 new_index)
        {
            Assert(new_index < source_size, 
                "new_index is stepping outside of source buffer"
            )
            peeked_char_index = new_index;
        };
    };
};