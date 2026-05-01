#include <clua_lexer/lexer.hpp>

#include <iostream>
#include <string>
#include <cassert>

using namespace CLua;

template<Common::uint64 TokenCount>
struct Test {
    const char* name;
    const char* input;
    TokenType expected_types[TokenCount];
    Common::uint64 expected_offsets[TokenCount];
    Common::uint64 expected_lengths[TokenCount];
    bool expect_error = false;
    ErrorCode expected_error;
};

template<Common::uint64 TokenCount>
void run_test(const Test<TokenCount>& test)
{
    std::cout << "[TEST] " << test.name << std::endl;

    Common::Source source(
        reinterpret_cast<unsigned char*>(const_cast<char*>(test.input)),
        std::strlen(test.input)
    );

    CLua::Lexer lexer(source);

    for (Common::uint64 i = 0; i < TokenCount; ++i)
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
            TokenType::Identifier,
            TokenType::EndOfFile
        },
        { 0, 14 },
        { 14, 1 }
    };

    Test<4> IDENTIFIER_WHITESPACE_IDENTIFIER {
        "identifier whitespace identifier",
        "foo bar",
        {
            TokenType::Identifier,
            TokenType::Whitespace,
            TokenType::Identifier,
            TokenType::EndOfFile
        },
        { 0, 3, 4, 7 },
        { 3, 1, 3, 1 }
    };

    Test<6> NUMBERS {
        "decimal hex binary",
        "12 0xFF 0b101",
        {
            TokenType::Numeric,
            TokenType::Whitespace,
            TokenType::Numeric,
            TokenType::Whitespace,
            TokenType::Numeric,
            TokenType::EndOfFile
        },
        { 0, 2, 3, 7, 8, 13 },
        { 2, 1, 4, 1, 5, 1 }
    };

    Test<2> MULTI_WHITESPACE {
        "multi whitespace",
        " \t  ",
        {
            TokenType::Whitespace,
            TokenType::EndOfFile
        },
        { 0, 4 },
        { 4, 1 }
    };

    Test<3> INLINE_COMMENT {
        "inline comment",
        "// hello world\n",
        {
            TokenType::Comment,
            TokenType::NewLine,
            TokenType::EndOfFile,
        },
        { 0, 14, 15 },
        { 14, 1, 1 }
    };

    Test<4> UNCLOSED_BLOCK_COMMENT {
        "unclosed block comment",
        "wdadwad122e312 /* dasd adwa",
        {
            TokenType::Identifier,
            TokenType::Whitespace,
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 14, 15, 27 },
        { 14, 1, 12, 1 },
        true,
        ErrorCode::UnclosedComment
    };


    Test<4> UNICODE_CHARACTERS_IN_IDENTIFIER {
        "unicode characters test",
        "asdxxź",
        {
            TokenType::Identifier,
            TokenType::Error,
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 5, 6, 7},
        { 5, 1, 1, 1},
        true,
        ErrorCode::UnexpectedCharacter
    };

    Test<2> STRING_LITERAL {
        "string literal",
        "\"hello\\nworld\"",
        {
            TokenType::String,
            TokenType::EndOfFile
        },
        { 0, 14 },
        { 14, 1 }
    };

    Test<2> CHAR_LITERAL {
        "char literal",
        "'a'",
        {
            TokenType::Char,
            TokenType::EndOfFile
        },
        { 0, 3 },
        { 3, 1 }
    };

    Test<2> DOT_PREFIX_FLOAT {
        "dot prefix float",
        ".5",
        {
            TokenType::Numeric,
            TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 }
    };

    /*
    Test<7> LUA_CAPTURE_AND_BLOCK {
        "lua capture and block",
        "@lua_embed []{print(\"x\")}",
        {
            TokenType::Symbol,
            TokenType::Identifier,
            TokenType::Whitespace,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::LuaBlock,
            TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 25 },
        { 1, 9, 1, 1, 1, 12, 1 }
    };
    */

    Test<2> EMPTY_STRING {
        "empty string",
        "\"\"",
        {
            TokenType::String,
            TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 }
    };

    Test<2> UNCLOSED_STRING {
        "unclosed string",
        "\"hello",
        {
            TokenType::Error, 
            TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 },
        true,
        ErrorCode::UnclosedString
    };

    Test<2> STRING_ENDING_WITH_ESCAPED_QUOTE {
        "escaped quote at end",
        "\"abc\\\"\"",
        {
            TokenType::String,
            TokenType::EndOfFile
        },
        { 0, 7 },
        { 7, 1 },
    };

    Test<2> STRING_DANGLING_BACKSLASH {
        "dangling backslash",
        "\"abc\\",
        {
            TokenType::Error, 
            TokenType::EndOfFile
        },
        { 0, 5 },
        { 5, 1 },
        true,
        ErrorCode::UnclosedString
    };

    Test<2> EMPTY_CHAR {
        "empty char",
        "''",
        {
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 },
        true,
        ErrorCode::InvalidCharCode
    };

    Test<2> MULTI_CHAR_LITERAL {
        "multi char literal",
        "'ab'",
        {
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 4 },
        { 4 , 1},
        true,
        ErrorCode::TooLongChar,
    };

    Test<2> ESCAPED_CHAR_LITERAL {
        "escaped char",
        "'\\n'",
        {
            TokenType::Char,
            TokenType::EndOfFile
        },
        { 0, 4 },
        { 4, 1 }
    };

    Test<2> JUST_DOT {
        "just dot",
        ".",
        {
            TokenType::Symbol,
            TokenType::EndOfFile
        },
        { 0, 1 },
        { 1, 1 }
    };

    Test<3> DOT_IDENTIFIER {
        "dot then identifier",
        ".abc",
        {
            TokenType::Symbol,
            TokenType::Identifier,
            TokenType::EndOfFile
        },
        { 0, 1, 4 },
        { 1, 3, 1 }
    };

    Test<2> TRAILING_DOT_FLOAT {
        "trailing dot float",
        "5.",
        {
            TokenType::Numeric,
            TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 }
    };

    Test<3> DOUBLE_DOT {
        "double dot",
        "1..2",
        {
            TokenType::Numeric, //1.
            TokenType::Numeric, //.2
            TokenType::EndOfFile 
        },
        { 0, 2, 4 },
        { 2, 2, 1 }
    };

    Test<2> LEADING_ZERO_NUMBER {
        "leading zero",
        "000123",
        {
            TokenType::Numeric,
            TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 }
    };

    Test<2> UNDERSCORE_IDENTIFIER {
        "underscore identifier",
        "_value",
        {
            TokenType::Identifier,
            TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 }
    };

    Test<2> IDENTIFIER_NUMBER {
        "identifier number",
        "abc123",
        {
            TokenType::Identifier,
            TokenType::EndOfFile
        },
        { 0, 6 },
        { 6, 1 }
    };

    Test<2> INLINE_COMMENT_EOF {
        "inline comment eof",
        "// comment",
        {
            TokenType::Comment,
            TokenType::EndOfFile
        },
        { 0, 10 },
        { 10, 1 }
    };

    /*
    Test<7> LUA_EMPTY_BLOCK {
        "lua empty block",
        "@lua_embed []{}",
        {
            TokenType::Symbol,
            TokenType::Identifier,
            TokenType::Whitespace,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::LuaBlock,
            TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 15 },
        { 1, 9, 1, 1, 1, 2, 1 }
    };

    Test<7> LUA_BRACE_INSIDE_STRING {
        "lua brace inside string",
        "@lua_embed []{print(\"{\")}",
        {
            TokenType::Symbol,
            TokenType::Identifier,
            TokenType::Whitespace,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::LuaBlock,
            TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 25 },
        { 1, 9, 1, 1, 1, 12, 1 }
    };

    Test<7> LUA_UNTERMINATED {
        "lua unterminated",
        "@lua_embed []{print(1)",
        {
            TokenType::Symbol,
            TokenType::Identifier,
            TokenType::Whitespace,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 1, 10, 11, 12, 13, 22},
        { 1, 9, 1, 1, 1, 9, 1 },

        true,
        ErrorCode::UnclosedLuaBlock
    };
    */

    Test<1> EMPTY_INPUT {
        "empty input",
        "",
        {
            TokenType::EndOfFile
        },
        { 0 },
        { 1 }
    };

    Test<4> MULTIPLE_COMMENTS {
        "multiple inline comments",
        "// a\n// b\n",
        {
            TokenType::Comment,
            TokenType::NewLine,
            TokenType::Comment,
            TokenType::NewLine
        },
        { 0,4,5,9 },
        { 4,1,4,1 }
    };

    Test<2> MIXED_WHITESPACE {
        "mixed whitespace",
        " \t \t ",
        {
            TokenType::Whitespace,
            TokenType::EndOfFile
        },
        { 0, 5 },
        { 5, 1 }
    };

    Test<6> SYMBOL_CLUSTER {
        "symbol cluster",
        "+-*/()",
        {
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::Symbol,
            TokenType::Symbol
        },
        { 0,1,2,3,4,5 },
        { 1,1,1,1,1,1 }
    };

    Test<2> ESCAPED_QUOTE_CHAR {
        "escaped quote char",
        "'\\''",
        {
            TokenType::Char,
            TokenType::EndOfFile
        },
        { 0, 4 },
        { 4, 1 }
    };

    Test<3> MULTIPLE_DOTS_FLOAT {
        "multiple dots float",
        "1.2.3",
        {
            TokenType::Numeric,
            TokenType::Numeric,
            TokenType::EndOfFile
        },
        { 0, 3, 5 },
        { 3, 2, 1 }
    };

    Test<2> INVALID_BINARY {
        "invalid binary",
        "0b",
        {
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 },
        true,
        ErrorCode::MalformedNumber
    };

    Test<2> INVALID_HEX {
        "invalid hex",
        "0x",
        {   
            TokenType::Error,
            TokenType::EndOfFile
        },
        { 0, 2 },
        { 2, 1 },
        true,
        ErrorCode::MalformedNumber
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
    //run_test_luau(LUA_CAPTURE_AND_BLOCK);

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

    /*
    run_test_luau(LUA_EMPTY_BLOCK);
    run_test_luau(LUA_BRACE_INSIDE_STRING);
    run_test_luau(LUA_UNTERMINATED);
    */

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