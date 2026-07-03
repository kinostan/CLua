#include <common/language_processing/lexer.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>

using namespace Common::Lexer;

struct LexerTestCase {
    const char* name;
    const char* input;
    std::vector<TokenType> expected_types;
    std::vector<Common::uint64> expected_offsets;
    std::vector<Common::uint64> expected_lengths;
    bool expect_error = false;
    Error expected_error = static_cast<Error>(ErrorCode::None);
};

void run_test(const LexerTestCase& test)
{
    std::cout << "[TEST] " << test.name << " ... ";

    Common::Source source(
        reinterpret_cast<unsigned char*>(const_cast<char*>(test.input)),
        std::strlen(test.input)
    );

    Lexer lexer(source);
    size_t token_count = test.expected_types.size();

    assert(test.expected_offsets.size() == token_count && "Expected offsets size mismatch");
    assert(test.expected_lengths.size() == token_count && "Expected lengths size mismatch");

    for (size_t i = 0; i < token_count; ++i)
    {
        auto is_end = i == token_count - 1;

        if (test.expected_types[i] == TokenType::EndOfFile)
        {
            if (lexer.peek_next_token().token_type == TokenType::EndOfFile)
            {
                break;
            };
        };

        auto token = lexer.process_next_token();

        if (token.token_type != test.expected_types[i] || 
            token.offset != test.expected_offsets[i] || 
            token.length != test.expected_lengths[i]) 
        {
            std::cout << "FAILED!\n";
            std::cerr << "  Mismatched token at index [" << i << "]:\n"
                      << "  Expected: Type=" << static_cast<int>(test.expected_types[i])
                      << ", Offset=" << test.expected_offsets[i]
                      << ", Length=" << test.expected_lengths[i] << "\n"
                      << "  Got     : Type=" << static_cast<int>(token.token_type)
                      << ", Offset=" << token.offset
                      << ", Length=" << token.length << "\n";
            assert(false);
        }
    }

    if (test.expect_error)
    {
        assert(lexer.get_current_error().error_code == test.expected_error.error_code);
    }

    std::cout << "OK\n";
}

int main()
{
    std::vector<LexerTestCase> test_suite = {
        {
            "identifier only",
            "wdadwad122e312",
            { TokenType::Word, TokenType::Numeric, TokenType::Word, TokenType::Numeric, TokenType::EndOfFile },
            { 0, 7, 10, 11, 14 },
            { 7, 3,  1,  3,  1 }
        },
        {
            "identifier whitespace identifier",
            "foo bar",
            { TokenType::Word, TokenType::Whitespace, TokenType::Word, TokenType::EndOfFile },
            { 0, 3, 4, 7 },
            { 3, 1, 3, 1 }
        },
        {
            "decimal hex binary",
            "12 0xFF 0b101",
            { 
                TokenType::Numeric, TokenType::Whitespace, 
                TokenType::Numeric, TokenType::Word, TokenType::Whitespace, 
                TokenType::Numeric, TokenType::Word, TokenType::Numeric, TokenType::EndOfFile 
            },
            { 0, 2, 3, 4, 7, 8, 9, 10, 13 },
            { 2, 1, 1, 3, 1, 1, 1, 3, 1 }
        },
        {
            "multi whitespace",
            " \t  ",
            { TokenType::Whitespace, TokenType::EndOfFile },
            { 0, 4 },
            { 4, 1 }
        },
        {
            "inline comment",
            "// hello\n",
            { TokenType::Symbol, TokenType::Symbol, TokenType::Whitespace, TokenType::Word, TokenType::NewLine, TokenType::EndOfFile },
            { 0, 1, 2, 3, 8, 9 },
            { 1, 1, 1, 5, 1, 1 }
        },
        {
            "unclosed block comment",
            "wdadwad122e312 /* ",
            { TokenType::Word, TokenType::Numeric, TokenType::Word, TokenType::Numeric, TokenType::Whitespace, TokenType::Symbol, TokenType::Symbol, TokenType::Whitespace },
            { 0, 7, 10, 11, 14, 15, 16, 17 },
            { 7, 3,  1,  3,  1,  1,  1,  1 }
        },
        {
            "unicode characters test",
            "asdxxź",
            { TokenType::Word, TokenType::UnicodeSequence, TokenType::EndOfFile },
            { 0, 5, 7 }, // 'ź' takes 2 bytes in UTF-8 execution character sets
            { 5, 2, 1 }
        },
        {
            "string literal structure",
            "\"hi\"",
            { TokenType::Symbol, TokenType::Word, TokenType::Symbol, TokenType::EndOfFile },
            { 0, 1, 3, 4 },
            { 1, 2, 1, 1 }
        },
        {
            "char literal structure",
            "'a'",
            { TokenType::Symbol, TokenType::Word, TokenType::Symbol, TokenType::EndOfFile },
            { 0, 1, 2, 3 },
            { 1, 1, 1, 1 }
        },
        {
            "dot prefix float",
            ".5",
            { TokenType::Symbol, TokenType::Numeric, TokenType::EndOfFile },
            { 0, 1, 2 },
            { 1, 1, 1 }
        },
        {
            "just dot",
            ".",
            { TokenType::Symbol, TokenType::EndOfFile },
            { 0, 1 },
            { 1, 1 }
        },
        {
            "dot then identifier",
            ".abc",
            { TokenType::Symbol, TokenType::Word, TokenType::EndOfFile },
            { 0, 1, 4 },
            { 1, 3, 1 }
        },
        {
            "trailing dot float",
            "5.",
            { TokenType::Numeric, TokenType::Symbol, TokenType::EndOfFile },
            { 0, 1, 2 },
            { 1, 1, 1 }
        },
        {
            "double dot",
            "1..2",
            { TokenType::Numeric, TokenType::Symbol, TokenType::Symbol, TokenType::Numeric, TokenType::EndOfFile },
            { 0, 1, 2, 3, 4 },
            { 1, 1, 1, 1, 1 }
        },
        {
            "leading zero",
            "000123",
            { TokenType::Numeric, TokenType::EndOfFile },
            { 0, 6 },
            { 6, 1 }
        },
        {
            "underscore identifier",
            "_value",
            { TokenType::Word, TokenType::EndOfFile }, // Matches your regex: [a-zA-Z_]
            { 0, 6 },
            { 6, 1 }
        },
        {
            "empty input",
            "",
            { TokenType::EndOfFile },
            { 0 },
            { 1 }
        },
        {
            "symbol cluster",
            "+-*/()",
            { TokenType::Symbol, TokenType::Symbol, TokenType::Symbol, TokenType::Symbol, TokenType::Symbol, TokenType::Symbol },
            { 0, 1, 2, 3, 4, 5 },
            { 1, 1, 1, 1, 1, 1 }
        }
    };

    for (const auto& test : test_suite)
    {
        run_test(test);
    }

    std::cout << "\nAll literal token domain tests passed.\n";
    return 0;
}