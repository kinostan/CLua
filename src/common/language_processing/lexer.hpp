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
        void consume_numbers()
        {
            auto current_char = lexer_context.source.see_current();
            auto char_type = get_char_type_from_char(current_char); 

            while (char_type == CharType::Numeric)
            {
                lexer_context.source.consume();

                current_char = lexer_context.source.see_current();
                char_type = get_char_type_from_char(current_char);
            };
        };

        void consume_letters()
        {
            auto current_char = lexer_context.source.see_current();
            auto char_type = get_char_type_from_char(current_char); 

            while (char_type == CharType::Word)
            {
                lexer_context.source.consume();

                current_char = lexer_context.source.see_current();
                char_type = get_char_type_from_char(current_char);
            };
        };

        private:
        TokenGeneric get_next_token()
        {
            Common::uint64 start = lexer_context.source.index;
            auto current_char = lexer_context.source.see_current();
            auto char_type = CharTable::get_char_type_from_char(current_char);
            TokenType token_type = TokenType::None; 

            switch (char_type)
            {
                case CharTable::CharType::Word:
                case CharTable::CharType::Numeric:
                {
                    consume_letters();
                    token_type = TokenType::Word; 
                    break;
                }
                case CharTable::CharType::Symbol:
                {
                    lexer_context.source.consume();
                    token_type = TokenType::Symbol;
                    break;
                }
                case CharTable::CharType::Whitespace:
                    lexer_context.source.consume();
                    token_type = TokenType::Whitespace; 
                case CharTable::CharType::Newline:
                {
                    lexer_context.source.consume();
                    token_type = TokenType::NewLine; 
                    break;
                }
                case CharTable::CharType::EndOfFile:
                {
                    token_type = TokenType::EndOfFile;
                    break;
                }
                case CharTable::CharType::Unicode:
                {
                    lexer_context.source.consume(); 
                    
                    lexer_context.record_error(ErrorCode::InvalidByte);
                    token_type = lexer_context.ultimate_token_type;
                    break;
                }
                case CharTable::CharType::Unrecognized:
                {
                    lexer_context.source.consume(); 
                    
                    lexer_context.record_error(ErrorCode::InvalidByte);
                    token_type = lexer_context.ultimate_token_type;
                    break;
                }
                default:
                    break;
            }

            LAssert(
                token_type != TokenType::None,
                "Unexpected behaviour from the lexer, because it didn't handle all char types"
            );

            lexer_context.original_token_type = token_type;
            lexer_context.ultimate_token_type = token_type;

            Common::uint64 end = lexer_context.source.index;
            Common::uint64 length = end - start;

            TokenGeneric token;
            token.token_type = lexer_context.ultimate_token_type;
            token.offset = start;
            token.length = length;

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
        
        LexerContext& get_lexer_context()
        {
            return lexer_context;
        };

        TokenGeneric process_next_token()
        {
            lexer_context.token_enter();
            return get_next_token();
        };

        const Error get_current_error()
        {
            return lexer_context.get_current_error();
        };
    };
}   
