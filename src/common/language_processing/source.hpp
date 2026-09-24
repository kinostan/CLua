#pragma once

#include <common/base.hpp>

#include <debugger/debugger.hpp>

#include <vector>

namespace Common {
    struct SourceView {
        unsigned char* source_buffer;
        uint64 source_size;
    };

    class Source {
        public:
        uint64 peeked_char_index;
        uint64 source_size;    
        private:   
        unsigned char* source_buffer;

        public:
            
        Source() = default;
        Source(unsigned char* source_buffer, uint64 source_size) : 
        peeked_char_index(0),
        source_size(source_size), 
        source_buffer(source_buffer)
        {
            Assert(source_buffer,
                "Source buffer must exist"
            );
        };

        [[nodiscard]] std::string_view slice_string(const uint64 offset,uint64 size) const {
            return {
                reinterpret_cast<char*>(source_buffer + offset),
                size
            };
        };

        Source slice(const uint64 start_index,uint64 length)
        {
            Assert(length > 0,
                "length must be greater than 0"
            )
            uint64 end_index = start_index + length;
            Assert(
                end_index <= source_size,
                "broken assumption that end_index <= source_size is true"
            )
            return {source_buffer + start_index,length};
        };

        Source slice(const uint64 start_index)
        {
            Assert(
                source_size > start_index,
                "source_size > start_index is not true"
            )

            return slice(start_index,source_size-start_index);
        };

        [[nodiscard]] unsigned char* get_source_buffer() const {
            return source_buffer;
        };

        [[nodiscard]] bool can_consume_sentinel(const uint64 consume_size = 1) const
        {
            //source_size, because the additional character is a null terminator
            return peeked_char_index + (consume_size - 1) <= source_size;
        };

        [[nodiscard]] bool can_consume(const uint64 consume_size = 1) const
        {
            return peeked_char_index + (consume_size - 1) < source_size;
        };

        void consume(uint64 consume_size = 1)
        {
            Assert(
                can_consume_sentinel(consume_size),
                "index is reading beyond the source_buffer"
            );

            peeked_char_index += consume_size;
        };

        [[nodiscard]] unsigned char see_current() const {
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

        [[nodiscard]] bool can_peek_sentinel(const uint64 peek_offset = 0) const noexcept
        {  
            return (peeked_char_index+peek_offset) <= source_size;
        };

        [[nodiscard]] bool can_peek(const uint64 peek_offset = 0) const noexcept
        {
                return (peeked_char_index+peek_offset) < source_size;
        };

        [[nodiscard]] unsigned char peek(const uint64 peek_offset = 0) const {
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

        void set_peek_index(const uint64 new_index)
        {
            Assert(new_index < source_size, 
                "new_index is stepping outside of source buffer"
            )
            peeked_char_index = new_index;
        };
    };
};