#pragma once
#include <common/base.hpp>

namespace Common::CharTable {
    enum class CharType : Common::uint8 {
        Symbol,
        Numeric,
        Word,
        Whitespace,
        NewLine,
        EndOfFile,
        Unicode,
        Unrecognized
    };

    struct CharTableWrapper {
        CharType data[256];
    };

    constexpr auto char_table = []() consteval {   
        CharTableWrapper wrapper = {};

        for (int i = 0; i < 256; i++) { wrapper.data[i] = CharType::Symbol; }
        for (int i = 1; i < 32; i++) { wrapper.data[i] = CharType::Unrecognized; }
        for (int i = 128; i <= 255; i++) { wrapper.data[i] = CharType::Unicode; }
        wrapper.data[0] = CharType::EndOfFile;

        wrapper.data[' ']  = CharType::Whitespace;
        wrapper.data['\t'] = CharType::Whitespace;
        wrapper.data['\r'] = CharType::Whitespace;
        wrapper.data['\v'] = CharType::Unrecognized;
        wrapper.data['\f'] = CharType::Unrecognized;
        wrapper.data['\n'] = CharType::NewLine;

        for (int lower_case = 'a'; lower_case <= 'z'; lower_case++) { wrapper.data[lower_case] = CharType::Word; }
        for (int upper_case = 'A'; upper_case <= 'Z'; upper_case++) { wrapper.data[upper_case] = CharType::Word; }
        for (int number = '0'; number <= '9'; number++) { wrapper.data[number] = CharType::Numeric; }

        return wrapper; 
    }();   

    [[nodiscard]] inline CharType get_char_type_from_char(unsigned char c) noexcept
    {
        return char_table.data[c];        
    };
};