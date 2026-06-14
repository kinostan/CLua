#pragma once

#include <common/base.hpp>
#include <common/language_processing/tokens.hpp>

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
        Common::uint64 index;
        Common::uint64 source_size;    
        private:   
        unsigned char* source_buffer;

        public:
        
        Source() = default;
        Source(unsigned char* source_buffer, Common::uint64 source_size) : source_buffer(source_buffer), source_size(source_size), index(0)
        {
            LAssert(source_buffer,
                "Source buffer must exist"
            );
        };

        std::string_view slice_string(Util::Lexer::TokenSpan& span)
        {
            auto string_index = span.start.offset;
            auto string_length = (span.end.offset + span.end.length) - string_index;
            
            return std::string_view(
                reinterpret_cast<char*>(source_buffer + string_index),
                string_length
            );
        };

        Source slice(Common::uint64 start_index,Common::uint64 length)
        {
            LAssert(length > 0,
                "length must be greater than 0"
            )
            Common::uint64 end_index = start_index + length;
            LAssert(
                end_index <= source_size,
                "broken assumption that end_index <= source_size is true"
            )
            return Source(source_buffer + start_index,length);
        };

        Source slice(Common::uint64 start_index)
        {
            LAssert(
                source_size > start_index,
                "source_size > start_index is not true"
            )

            return slice(start_index,source_size-start_index);
        };

        inline unsigned char* get_source_buffer()
        {
            return source_buffer;
        };

        inline bool can_consume_sentinel(Common::uint64 consume_distance = 1)
        {
            //source_size, because the additional character is a null terminator
            return index + consume_distance < source_size + 1;
        };

        inline bool can_consume(Common::uint64 consume_distance = 1)
        {
            return index + consume_distance < source_size;
        };

        inline void consume(Common::uint64 consume_distance = 1)
        {
            LAssert(
                can_consume_sentinel(consume_distance),
                "index is reading beyond the source_buffer"
            );
            index += consume_distance;
        };

        inline unsigned char see_current()
        {
            LAssert(
                can_consume_sentinel(),
                "index is reading beyond the source_buffer"
            );
            if (!can_consume())
            {
                return '\0';
            };
            return source_buffer[index];
        };

        inline bool can_peek_sentinel(Common::uint64 peek_distance) const noexcept
        {  
            return (index+peek_distance) < source_size + 1;
        };

        inline bool can_peek(Common::uint64 peek_distance = 1) const noexcept
        {
            return (index+peek_distance) < source_size;
        };

        inline unsigned char peek(Common::uint64 peek_distance = 1)
        {   
            LAssert(
                can_peek_sentinel(peek_distance),
                "Can't peek here"
            );
            if (!can_peek())
            {
                return (unsigned char)'\0';
            };
            return source_buffer[index+peek_distance];
        };
    
        inline void set_index(Common::uint64 new_index)
        {
            LAssert(new_index < source_size, 
                "new_index is stepping outside of source buffer"
            )
            index = new_index;
        };
    };
};