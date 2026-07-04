#pragma once

#include <common/base.hpp>
#include <common/metacpp.hpp>

#include "./source.hpp"
#include "./tokens.hpp"
#include "./char_table_type.hpp"

#include <debugger/debugger.hpp>

namespace Common::Lexer {
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
        Common::uint64 peek_index = 0;    
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
        bool match_word(unsigned char* word,Common::uint64 offset = 0)
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

            size_t i = offset;
            do {
                if (!source.can_peek(i))
                {
                    return false;
                };
                auto current_stream_char = source.peek(i);
                auto current_word_char = word[i - offset];
                
                if (current_stream_char != current_word_char)
                {
                    return false;
                };

                i++;
            } while(word[i - offset] != '\0');

            if (source.can_peek(i))
            {
                auto next_char_type = get_char_type_from_char(source.peek(i));
                if (next_char_type == CharType::Word)
                {
                    return false;
                };
            };
            
            return true;
        };

        bool match_symbols(unsigned char* symbols,Common::uint64 offset = 0)
        {
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

            size_t i = offset;
            do {
                if (!source.can_peek(i))
                {
                    return false;
                };
                auto current_stream_char = source.peek(i);
                auto current_word_char = symbols[i - offset];
                
                if (current_stream_char != current_word_char)
                {
                    return false;
                };

                i++;
            } while(symbols[i - offset] != '\0');

            if (source.can_peek(i))
            {
                auto next_char_type = get_char_type_from_char(source.peek(i));
                if (next_char_type == CharType::Symbol)
                {
                    return false;
                };
            };
            
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
            lexer_state = new_lexer_state;
            source.peeked_char_index = new_lexer_state.peek_index;

            LAssert(
                source.can_peek_sentinel(),
                "peek_index exceeds legal value"
            );
        };

        inline LexerState record_cursor()
        {
            lexer_state.peek_index = source.peeked_char_index;
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
        bool scan_unicode(Common::uint64& token_size)   
        {
            auto leader = lexer_context.source.see_current();

            int expected_bytes = 0;

            if (leader >= 0 && leader <= 127) {
                expected_bytes = 1;
            } else if (leader >= 194 && leader <= 223) {
                expected_bytes = 2;
            } else if (leader >= 224 && leader <= 239) {
                expected_bytes = 3;
            } else if (leader >= 240 && leader <= 244) {
                expected_bytes = 4;
            } else {
                token_size = 1;
                return false; 
            }

            token_size = 1;

            for (int i = 1; i < expected_bytes; ++i) {
                auto next_byte = lexer_context.source.peek(i);

                if (next_byte >= 128 && next_byte <= 191) {
                    token_size++;
                } else {
                    return false; 
                }
            }

            return true; // Successfully scanned a valid UTF-8 sequence
        }

        TokenGeneric scan_token_bounds()
        {
            Common::uint64 start_offset = lexer_context.source.peeked_char_index;
            Common::uint64 token_size = 0;

            auto current_char = lexer_context.source.peek(token_size);
            auto char_type = CharTable::get_char_type_from_char(current_char);
            TokenType token_type = TokenType::None;

            switch (char_type)
            {
                case CharTable::CharType::Word:
                {
                    lexer_context.original_token_type = TokenType::Word;
                    lexer_context.ultimate_token_type = TokenType::Word;

                    do {
                        token_size++;
                        char_type = CharTable::get_char_type_from_char(lexer_context.source.peek(token_size));
                    } while (char_type == CharType::Word);

                    token_type = TokenType::Word;
                    break;
                }
                case CharTable::CharType::Numeric:
                {
                    lexer_context.original_token_type = TokenType::Numeric;
                    lexer_context.ultimate_token_type = TokenType::Numeric;

                    do {
                        token_size++;
                        char_type = CharTable::get_char_type_from_char(lexer_context.source.peek(token_size));
                    } while (char_type == CharType::Numeric);
                    
                    token_type = TokenType::Numeric;
                    break;
                }
                case CharTable::CharType::Symbol:
                {
                    lexer_context.original_token_type = TokenType::Symbol;
                    lexer_context.ultimate_token_type = TokenType::Symbol;

                    token_size = 1;
                    token_type = TokenType::Symbol;
                    break;
                }
                case CharTable::CharType::Whitespace:
                {
                    lexer_context.original_token_type = TokenType::Whitespace;
                    lexer_context.ultimate_token_type = TokenType::Whitespace;

                    do {
                        token_size++;
                        char_type = CharTable::get_char_type_from_char(lexer_context.source.peek(token_size));
                    } while (char_type == CharType::Whitespace);

                    token_type = TokenType::Whitespace;
                    break;
                }
                case CharTable::CharType::NewLine:
                {
                    lexer_context.original_token_type = TokenType::NewLine;
                    lexer_context.ultimate_token_type = TokenType::NewLine;

                    token_size = 1;
                    token_type = TokenType::NewLine;
                    break;
                }
                case CharTable::CharType::EndOfFile:
                {
                    lexer_context.original_token_type = TokenType::EndOfFile;
                    lexer_context.ultimate_token_type = TokenType::EndOfFile;

                    token_size = 1;
                    token_type = TokenType::EndOfFile;
                    break;
                }
                case CharTable::CharType::Unicode:
                {
                    lexer_context.original_token_type = TokenType::UnicodeSequence;
                    lexer_context.ultimate_token_type = TokenType::UnicodeSequence;
            
                    auto is_byte_code_valid = scan_unicode(token_size);
                    token_type = TokenType::UnicodeSequence;
                    if (!is_byte_code_valid) [[unlikely]]
                    {
                        lexer_context.record_error(ErrorCode::UnrecognizedUnicodeSequence);
                        scan_unicode(token_size);
                    };
                    break;
                }
                case CharTable::CharType::Unrecognized:
                {
                    token_size = 1;
                    lexer_context.record_error(ErrorCode::InvalidByte);
                    token_type = TokenType::Error;
                    break;
                }
                default:
                    break;
            }

            LAssert(
                token_size > 0,
                "Unexpected code behaviour, peeked token size is 0"
            );
            LAssert(token_type != TokenType::None, "Lexer window calculation bypassed validation rules.");

            TokenGeneric token;
            token.token_type = lexer_context.ultimate_token_type;
            token.offset = start_offset;
            token.length = token_size;

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
                .peek_index = token.offset + token.length,
                .current_error = ErrorCode::None
            });
        };

        inline bool match_word(unsigned char* word,Common::uint64 offset = 0)
        {
            return lexer_context.match_word(word,offset);
        };

        inline bool match_symbols(unsigned char* symbols,Common::uint64 offset = 0)
        {
            return lexer_context.match_symbols(symbols,offset);
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

        TokenGeneric process_next_token()
        {
            lexer_context.token_enter();
            TokenGeneric token = scan_token_bounds();

            commit_token_window(token);
            return token;
        }

        const Error get_current_error()
        {
            return lexer_context.get_current_error();
        };
    };
}   
