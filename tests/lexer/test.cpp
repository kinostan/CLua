#include <lexer/lexer.hpp>

#include <iostream>
#include <string>
#include <cassert>

template<Util::uint64 TokenCount>
struct Test {
    const char* name;
    const char* input;
    Util::TokenType expected_types[TokenCount];
    Util::uint64 expected_offsets[TokenCount];
    Util::uint64 expected_lengths[TokenCount];
    bool expect_error = false;
    Util::ErrorCode expected_error;
};

template<Util::uint64 TokenCount>
void run_test(const Test<TokenCount>& test)
{
    std::cout << "[TEST] " << test.name << std::endl;

    Util::Source source(
        reinterpret_cast<unsigned char*>(const_cast<char*>(test.input)),
        std::strlen(test.input)
    );

    Util::Lexer lexer(source);

    for (Util::uint64 i = 0; i < TokenCount; ++i)
    {
        auto token = lexer.process_next_token();
        bool cond = token.token_type == test.expected_types[i] && token.offset == test.expected_offsets[i] && token.length == test.expected_lengths[i];

        assert(token.token_type == test.expected_types[i]);
        assert(token.offset == test.expected_offsets[i]);
        assert(token.length == test.expected_lengths[i]);
    }

    if (test.expect_error)
    {
        assert(lexer.get_current_error().error_code == test.expected_error);
    }

    std::cout << "  OK\n";
}

int main()
{
    Test<2> IDENTIFIER_ONLY {
        "identifier only",
        "wdadwad122e312",
        {
            Util::TokenType::Identifier,
            Util::TokenType::EndOfFile
        },
        { 0, 14 },
        { 14, 1 }
    };

    Test<4> IDENTIFIER_WHITESPACE_IDENTIFIER {
        "identifier whitespace identifier",
        "foo bar",
        {
            Util::TokenType::Identifier,
            Util::TokenType::Whitespace,
            Util::TokenType::Identifier,
            Util::TokenType::EndOfFile
        },
        { 0, 3, 4, 7 },
        { 3, 1, 3, 1 }
    };

    Test<6> NUMBERS {
        "decimal hex binary",
        "12 0xFF 0b101",
        {
            Util::TokenType::Numeric,
            Util::TokenType::Whitespace,
            Util::TokenType::Numeric,
            Util::TokenType::Whitespace,
            Util::TokenType::Numeric,
            Util::TokenType::EndOfFile
        },
        { 0, 2, 3, 7, 8, 13 },
        { 2, 1, 4, 1, 5, 1 }
    };

    Test<2> MULTI_WHITESPACE {
        "multi whitespace",
        " \t  ",
        {
            Util::TokenType::Whitespace,
            Util::TokenType::EndOfFile
        },
        { 0, 4 },
        { 4, 1 }
    };

    Test<3> INLINE_COMMENT {
        "inline comment",
        "// hello world\n",
        {
            Util::TokenType::Comment,
            Util::TokenType::NewLine,
            Util::TokenType::EndOfFile,
        },
        { 0, 14, 15 },
        { 14, 1, 1 }
    };

    Test<4> UNCLOSED_BLOCK_COMMENT {
        "unclosed block comment",
        "wdadwad122e312 /* dasd adwa",
        {
            Util::TokenType::Identifier,
            Util::TokenType::Whitespace,
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 14, 15, 27 },
        { 14, 1, 12, 1 },
        true,
        Util::ErrorCode::UnclosedComment
    };


    Test<4> UNICODE_CHARACTERS_IN_IDENTIFIER {
        "unicode characters test",
        "asdxxź",
        {
            Util::TokenType::Identifier,
            Util::TokenType::Error,
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 5, 6, 7},
        { 5, 1, 1, 1},
        true,
        Util::ErrorCode::UnexpectedCharacter
    };

    Test<2> STRING_LITERAL {
        "string literal",
        "\"hello\\nworld\"",
        {
            Util::TokenType::String,
            Util::TokenType::EndOfFile
        },
        { 0, 14 },
        { 14, 1 }
    };

    Test<2> CHAR_LITERAL {
        "char literal",
        "'a'",
        {
            Util::TokenType::Char,
            Util::TokenType::EndOfFile
        },
        { 0, 3 },
        { 3, 1 }
    };

    Test<2> DOT_PREFIX_FLOAT {
        "dot prefix float",
        ".5",
        {
            Util::TokenType::Numeric,
            Util::TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 }
    };

    Test<7> LUA_CAPTURE_AND_BLOCK {
        "lua capture and block",
        "@lua_embed []{print(\"x\")}",
        {
            Util::TokenType::Symbol,
            Util::TokenType::Identifier,
            Util::TokenType::Whitespace,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::LuaBlock,
            Util::TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 25 },
        { 1, 9, 1, 1, 1, 12, 1 }
    };

    Test<2> EMPTY_STRING {
        "empty string",
        "\"\"",
        {
            Util::TokenType::String,
            Util::TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 }
    };

    Test<2> UNCLOSED_STRING {
        "unclosed string",
        "\"hello",
        {
            Util::TokenType::Error, 
            Util::TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 },
        true,
        Util::ErrorCode::UnclosedString
    };

    Test<2> STRING_ENDING_WITH_ESCAPED_QUOTE {
        "escaped quote at end",
        "\"abc\\\"\"",
        {
            Util::TokenType::String,
            Util::TokenType::EndOfFile
        },
        { 0, 7 },
        { 7, 1 },
    };

    Test<2> STRING_DANGLING_BACKSLASH {
        "dangling backslash",
        "\"abc\\",
        {
            Util::TokenType::Error, 
            Util::TokenType::EndOfFile
        },
        { 0, 5 },
        { 5, 1 },
        true,
        Util::ErrorCode::UnclosedString
    };

    Test<2> EMPTY_CHAR {
        "empty char",
        "''",
        {
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 },
        true,
        Util::ErrorCode::InvalidCharCode
    };

    Test<2> MULTI_CHAR_LITERAL {
        "multi char literal",
        "'ab'",
        {
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 4 },
        { 4 , 1},
        true,
        Util::ErrorCode::TooLongChar,
    };

    Test<2> ESCAPED_CHAR_LITERAL {
        "escaped char",
        "'\\n'",
        {
            Util::TokenType::Char,
            Util::TokenType::EndOfFile
        },
        { 0, 4 },
        { 4, 1 }
    };

    Test<2> JUST_DOT {
        "just dot",
        ".",
        {
            Util::TokenType::Symbol,
            Util::TokenType::EndOfFile
        },
        { 0, 1 },
        { 1, 1 }
    };

    Test<3> DOT_IDENTIFIER {
        "dot then identifier",
        ".abc",
        {
            Util::TokenType::Symbol,
            Util::TokenType::Identifier,
            Util::TokenType::EndOfFile
        },
        { 0, 1, 4 },
        { 1, 3, 1 }
    };

    Test<2> TRAILING_DOT_FLOAT {
        "trailing dot float",
        "5.",
        {
            Util::TokenType::Numeric,
            Util::TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 }
    };

    Test<3> DOUBLE_DOT {
        "double dot",
        "1..2",
        {
            Util::TokenType::Numeric, //1.
            Util::TokenType::Numeric, //.2
            Util::TokenType::EndOfFile 
        },
        { 0, 2, 4 },
        { 2, 2, 1 }
    };

    Test<2> LEADING_ZERO_NUMBER {
        "leading zero",
        "000123",
        {
            Util::TokenType::Numeric,
            Util::TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 }
    };

    Test<2> UNDERSCORE_IDENTIFIER {
        "underscore identifier",
        "_value",
        {
            Util::TokenType::Identifier,
            Util::TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 }
    };

    Test<2> IDENTIFIER_NUMBER {
        "identifier number",
        "abc123",
        {
            Util::TokenType::Identifier,
            Util::TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 }
    };

    Test<2> INLINE_COMMENT_EOF {
        "inline comment eof",
        "// comment",
        {
            Util::TokenType::Comment,
            Util::TokenType::EndOfFile
        },
        { 0, 10 },
        { 10, 1 }
    };

    Test<7> LUA_EMPTY_BLOCK {
        "lua empty block",
        "@lua_embed []{}",
        {
            Util::TokenType::Symbol,
            Util::TokenType::Identifier,
            Util::TokenType::Whitespace,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::LuaBlock,
            Util::TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 15 },
        { 1, 9, 1, 1, 1, 2, 1 }
    };

    Test<7> LUA_BRACE_INSIDE_STRING {
        "lua brace inside string",
        "@lua_embed []{print(\"{\")}",
        {
            Util::TokenType::Symbol,
            Util::TokenType::Identifier,
            Util::TokenType::Whitespace,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::LuaBlock,
            Util::TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 25 },
        { 1, 9, 1, 1, 1, 12, 1 }
    };

    Test<7> LUA_UNTERMINATED {
        "lua unterminated",
        "@lua_embed []{print(1)",
        {
            Util::TokenType::Symbol,
            Util::TokenType::Identifier,
            Util::TokenType::Whitespace,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 22},
        { 1, 9, 1, 1, 1, 9, 1 },

        true,
        Util::ErrorCode::UnclosedLuaBlock
    };

    Test<1> EMPTY_INPUT {
        "empty input",
        "",
        {
            Util::TokenType::EndOfFile
        },
        { 0 },
        { 1 }
    };

    Test<4> MULTIPLE_COMMENTS {
        "multiple inline comments",
        "// a\n// b\n",
        {
            Util::TokenType::Comment,
            Util::TokenType::NewLine,
            Util::TokenType::Comment,
            Util::TokenType::NewLine
        },
        { 0,4,5,9 },
        { 4,1,4,1 }
    };

    Test<2> MIXED_WHITESPACE {
        "mixed whitespace",
        " \t \t ",
        {
            Util::TokenType::Whitespace,
            Util::TokenType::EndOfFile
        },
        { 0, 5 },
        { 5, 1 }
    };

    Test<6> SYMBOL_CLUSTER {
        "symbol cluster",
        "+-*/()",
        {
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol,
            Util::TokenType::Symbol
        },
        { 0,1,2,3,4,5 },
        { 1,1,1,1,1,1 }
    };

    Test<2> ESCAPED_QUOTE_CHAR {
        "escaped quote char",
        "'\\''",
        {
            Util::TokenType::Char,
            Util::TokenType::EndOfFile
        },
        { 0, 4 },
        { 4, 1 }
    };

    Test<3> MULTIPLE_DOTS_FLOAT {
        "multiple dots float",
        "1.2.3",
        {
            Util::TokenType::Numeric,
            Util::TokenType::Numeric,
            Util::TokenType::EndOfFile
        },
        { 0, 3, 5 },
        { 3, 2, 1 }
    };

    Test<2> INVALID_BINARY {
        "invalid binary",
        "0b",
        {
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 },
        true,
        Util::ErrorCode::MalformedNumber
    };

    Test<2> INVALID_HEX {
        "invalid hex",
        "0x",
        {   
            Util::TokenType::Error,
            Util::TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 },
        true,
        Util::ErrorCode::MalformedNumber
    };

    run_test(IDENTIFIER_ONLY);
    run_test(IDENTIFIER_WHITESPACE_IDENTIFIER);
    run_test(NUMBERS);
    run_test(MULTI_WHITESPACE);
    run_test(INLINE_COMMENT);
    run_test(UNCLOSED_BLOCK_COMMENT);
    run_test(UNICODE_CHARACTERS_IN_IDENTIFIER);
    run_test(STRING_LITERAL);
    run_test(CHAR_LITERAL);
    run_test(DOT_PREFIX_FLOAT);
    run_test(LUA_CAPTURE_AND_BLOCK);

    run_test(EMPTY_STRING);
    run_test(UNCLOSED_STRING);
    run_test(STRING_ENDING_WITH_ESCAPED_QUOTE);
    run_test(STRING_DANGLING_BACKSLASH);

    run_test(EMPTY_CHAR);
    run_test(MULTI_CHAR_LITERAL);
    run_test(ESCAPED_CHAR_LITERAL);

    run_test(JUST_DOT);
    run_test(DOT_IDENTIFIER);
    run_test(TRAILING_DOT_FLOAT);
    run_test(DOUBLE_DOT);
    run_test(LEADING_ZERO_NUMBER);

    run_test(UNDERSCORE_IDENTIFIER);
    run_test(IDENTIFIER_NUMBER);

    run_test(INLINE_COMMENT_EOF);

    run_test(LUA_EMPTY_BLOCK);
    run_test(LUA_BRACE_INSIDE_STRING);
    run_test(LUA_UNTERMINATED);

    run_test(EMPTY_INPUT);

    run_test(MULTIPLE_COMMENTS);
    run_test(MIXED_WHITESPACE);
    run_test(SYMBOL_CLUSTER);
    run_test(ESCAPED_QUOTE_CHAR);
    run_test(MULTIPLE_DOTS_FLOAT);

    run_test(INVALID_BINARY);
    run_test(INVALID_HEX);

    std::cout << "\nAll lexer tests passed.\n";
    return 0;
}