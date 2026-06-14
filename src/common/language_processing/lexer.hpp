#pragma once

#include <common/base.hpp>
#include <common/metacpp.hpp>

#include "./source.hpp"
#include "./tokens.hpp"
#include "./char_table_type.hpp"

#include <debugger/debugger.hpp>

namespace Util::Lexer {
    using Source = Common::Source;
    using CharType = CharTable::CharType;
    using CharTable::get_char_type_from_char;

    enum class ErrorCode: Common::uint8 {
        None,
        UnrecognizedUnicodeSequence,
        InvalidByte,
    };

    struct Error {
        ErrorCode error_code;
    };

    struct LexerState { 
        Common::uint64 cursor_index = 0;    
        Error current_error;
    };

    class LexerContext {
        private:
        bool emitted = false;
        public:

        LexerState lexer_state;

        Source source;

        TokenType ultimate_token_type = TokenType::Error;
        TokenType original_token_type = ultimate_token_type; //this variable is strictly for recover if user chooses to do so

        LexerContext() = default;
        LexerContext(Source& source): source(source)
        {};

        inline void token_enter()
        {
            emitted = false;
        };

        private:
        inline void on_emit(){
            LAssert(
                !emitted,
                "trying to emit hint multiple times within the same token"
            )

            emitted = true;
        }
        
        public:
        bool match_word(unsigned char* word)
        {
            LAssert(
                //Are symbol characters valid
                [&word](){
                    auto count = 0;
                    while (word[count] != '\0')
                    {
                        auto char_type = get_char_type_from_char(word[count]);
                        if (char_type != CharType::Word)
                        {
                            return false;
                        };
                        count++;
                    };
                    return true;
                }() ,
                "Word is invalid, it contains non-word characters"
            );

            size_t i = 0;
            do {
                if (!source.can_peek(i))
                {
                    return false;
                };
                auto current_stream_char = source.peek(i);
                auto current_word_char = word[i];
                
                if (current_stream_char != current_word_char)
                {
                    return false;
                };

                i++;
            } while(word[i] != '\0');
            
            return true;
        };

        bool match_symbols(unsigned char* symbols)
        {
            /*
            
            */

            LAssert(
                //Are symbol characters valid
                [&symbols](){
                    auto count = 0;
                    while (symbols[count] != '\0')
                    {
                        auto char_type = get_char_type_from_char(symbols[count]);
                        if (char_type != CharType::Symbol)
                        {
                            return false;
                        };
                        count++;
                    };
                    return true;
                }(),
                "Symbols stream is invalid, it contains non-symbol characters"
            )

            size_t i = 0;
            do {
                if (!source.can_peek(i))
                {
                    return false;
                };
                auto current_stream_char = source.peek(i);
                auto current_word_char = symbols[i];
                
                if (current_stream_char != current_word_char)
                {
                    return false;
                };

                i++;
            } while(symbols[i] != '\0');
            
            return true;
        };

        constexpr inline bool has_emitted_report()
        {
            return emitted;
        };

        inline void record_error(ErrorCode error_code)
        {
            on_emit();

            Error error;
            error.error_code = error_code;
            lexer_state.current_error = error;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<ErrorToken>::value;
        };

        const Error get_current_error()
        {
            return lexer_state.current_error;
        };

        inline void set_cursor(LexerState new_lexer_state)
        {
            LAssert(
                new_lexer_state.cursor_index < source.source_size,
                "cursor_index exceeds legal value"
            );

            lexer_state = new_lexer_state;
            source.index = new_lexer_state.cursor_index;
        };

        inline LexerState record_cursor()
        {
            lexer_state.cursor_index = source.index;
            return lexer_state;
        };
    };

    class Lexer
    {
        private:
        LexerContext lexer_context;


        public:
        Lexer() = default;
        Lexer(Source& source)
        {
            lexer_context = LexerContext(source);
        };

        private:

        TokenGeneric scan_token_bounds()
        {
            Common::uint64 start_offset = lexer_context.source.index;
            Common::uint64 scan_index = 0;

            auto current_char = lexer_context.source.peek(scan_index);
            auto char_type = CharTable::get_char_type_from_char(current_char);
            TokenType token_type = TokenType::None;

            switch (char_type)
            {
                case CharTable::CharType::Word:
                case CharTable::CharType::Numeric:
                {
                    while (char_type == CharType::Word || char_type == CharType::Numeric) {
                        scan_index++;
                        char_type = CharTable::get_char_type_from_char(lexer_context.source.peek(scan_index));
                    }
                    token_type = TokenType::Word;
                    break;
                }
                case CharTable::CharType::Symbol:
                {
                    scan_index = 1;
                    token_type = TokenType::Symbol;
                    break;
                }
                case CharTable::CharType::Whitespace:
                {
                    while (char_type == CharType::Whitespace) {
                        scan_index++;
                        char_type = CharTable::get_char_type_from_char(lexer_context.source.peek(scan_index));
                    }
                    token_type = TokenType::Whitespace;
                    break;
                }
                case CharTable::CharType::Newline:
                {
                    scan_index = 1;
                    token_type = TokenType::NewLine;
                    break;
                }
                case CharTable::CharType::EndOfFile:
                {
                    scan_index = 1; 
                    token_type = TokenType::EndOfFile;
                    break;
                }
                case CharTable::CharType::Unicode:
                case CharTable::CharType::Unrecognized:
                {
                    scan_index = 1;
                    lexer_context.record_error(ErrorCode::InvalidByte);
                    token_type = TokenType::Error;
                    break;
                }
                default:
                    break;
            }

            LAssert(token_type != TokenType::None, "Lexer window calculation bypassed validation rules.");

            lexer_context.original_token_type = token_type;
            lexer_context.ultimate_token_type = token_type;

            TokenGeneric token;
            token.token_type = lexer_context.ultimate_token_type;
            token.offset = start_offset;
            token.length = scan_index;

            return token;
        };
        public:
        inline LexerState record_cursor()
        {
            return lexer_context.record_cursor();
        };

        inline void set_cursor(LexerState lexer_state)
        {
            lexer_context.set_cursor(lexer_state);
        };

        inline void commit_token_window(TokenGeneric token)
        {
            lexer_context.set_cursor(
                LexerState{
                .cursor_index = token.offset + token.length,
                .current_error = ErrorCode::None
            });
        };
        
        LexerContext& get_lexer_context()
        {
            return lexer_context;
        };

        TokenGeneric peek_next_token()
        {
            lexer_context.token_enter();
            return scan_token_bounds();
        };

        const Error get_current_error()
        {
            return lexer_context.get_current_error();
        };
    };
}   
