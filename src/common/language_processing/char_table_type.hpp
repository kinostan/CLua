#pragma once
#include <common/base.hpp>

namespace Util::CharTable {
    enum class CharType : Common::uint8 {
        Symbol,
        Numeric,
        Word,
        Whitespace,
        Newline,
        EndOfFile,
        Unicode,
        Unrecognized
    };

    consteval auto char_table = [](){   
        CharType table[256] = {};

        for (int i = 0; i < 256; i++) {
            table[i] = CharType::Symbol;
        }

        for (int i = 1; i < 32; i++) {
            table[i] = CharType::Unrecognized;
        }

        for (int i = 128; i <= 255; i++) {
            table[i] = CharType::Unicode;
        }
        table[0] = CharType::EndOfFile;

        table[' ']  = CharType::Whitespace;
        table['\t'] = CharType::Whitespace;
        table['\r'] = CharType::Whitespace;
        table['\v'] = CharType::Unrecognized;
        table['\f'] = CharType::Unrecognized;

        table['\n'] = CharType::Newline;

        table['_'] = CharType::Word;

        for (int lower_case = 'a'; lower_case <= 'z'; lower_case++) {
            table[lower_case] = CharType::Word;
        }
        for (int upper_case = 'A'; upper_case <= 'Z'; upper_case++) {
            table[upper_case] = CharType::Word;
        }

        for (int number = '0'; number <= '9'; number++) {
            table[number] = CharType::Numeric;
        }

        return table;
    }();

    [[nodiscard]] inline CharType get_char_type_from_char(unsigned char c) noexcept
    {
        return char_table[c];        
    };
};