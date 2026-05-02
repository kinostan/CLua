#pragma once

#include <common/base.hpp>
#include <common/source.hpp>
#include <common/metacpp.hpp>

#include <common/clua/tokens.hpp>
#include <common/clua/symbol_classifier.hpp>
#include <common/clua/keyword_classifier.hpp>

#include <debugger/debugger.hpp>

namespace CLua {
    using Source = Common::Source;

    enum class Error: Common::uint8 {
        None,
        UnknownSymbol,
        UnexpectedCharacter,
        UnexpectedTokenType,
        InvalidByte,
        TruncatedUnicodeSequence,
        TruncatedNumberSequence,
        MalformedNumber,
        UnclosedComment,
        UnclosedString,
        UnclosedChar,
        InvalidCharCode,
        TooLongChar,
    };

    enum class NumberType: Common::uint8 {
        Integer,
        Float,
        None,
    };

    enum class NumberBase: Common::uint8  {
        Hexadecimal,
        Decimal,
        Binary,
        None,
    };

    struct NumberHint {
        NumberType number_type = NumberType::None;
        NumberBase number_base = NumberBase::None;
    };

    class TokenSpan {
        public:
        TokenGeneric start;
        TokenGeneric end;
        
        TokenSpan(TokenGeneric start,TokenGeneric end): start(start), end(end)
        {};
    };

    struct LexerState { 
        union {
            Common::uint64 current_number_integer = 0;
            Common::f64 current_number_fraction; 
            char current_char_value;
            SymbolKind current_symbol;  
            Keyword current_keyword;
            Error current_error;
        };
        Common::uint64 cursor_index = 0;
        NumberHint current_number = NumberHint();
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

        inline void record_error(Error error)
        {
            on_emit();

            lexer_state.current_error = error;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<ErrorToken>::value;
        };

        inline void record_number(NumberBase number_base, NumberType number_type, Common::uint64 number_integer,Common::f64 number_fraction = 0)
        {
            on_emit();

            LAssert(
                number_fraction >= 0,
                "lexer can't consume unary minus operator"
            );

            lexer_state.current_number_integer = number_integer;
            lexer_state.current_number_fraction = number_fraction;

            NumberHint number_hint;
            number_hint.number_base = number_base;
            number_hint.number_type = number_type;

            lexer_state.current_number = number_hint;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<NumericToken>::value;
        };

        inline void record_symbol(SymbolKind symbol)
        {
            on_emit();

            lexer_state.current_symbol = symbol;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<SymbolToken>::value;
        };

        inline void record_identifier(std::string_view identifier)
        {
            on_emit();

            auto keyword_type = get_keyword_type(identifier);
            lexer_state.current_keyword = keyword_type;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<IdentifierToken>::value;
        };

        inline void record_char_value(char char_value)
        {
            on_emit();

            lexer_state.current_char_value = char_value;
        };

        const Error get_current_error()
        {
            return lexer_state.current_error;
        };

        const NumberHint get_current_number_hint()
        {
            return lexer_state.current_number;
        };

        const SymbolKind get_current_symbol()
        {
            return lexer_state.current_symbol;
        };

        const Keyword get_current_keyword()
        {
            return lexer_state.current_keyword;
        };

        const Common::f64 get_current_fraction() 
        {
            LAssert(
                lexer_state.current_number_fraction >= 0,
                "invalid state"
            );

            return lexer_state.current_number_fraction;
        };

        const unsigned long long get_current_integer()
        {
            return lexer_state.current_number_integer;
        };

        const char get_current_char_value()
        {
            return lexer_state.current_char_value;
        };


        inline void set_cursor(LexerState new_lexer_state)
        {
            LAssert(
                new_lexer_state.cursor_index < source.source_size,
                "cursor_index exceeds legal value"
            );

            lexer_state = new_lexer_state;
        };

        inline LexerState record_cursor()
        {
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
        TokenGeneric get_next_token();
    
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

        const NumberHint get_current_number_hint()
        {
            return lexer_context.get_current_number_hint();
        };

        const SymbolKind get_current_symbol()
        {
            return lexer_context.get_current_symbol();
        };

        const Keyword get_current_keyword()
        {
            return lexer_context.get_current_keyword();
        };

        const Common::f64 get_current_fraction() 
        {
            return lexer_context.get_current_fraction();
        };

        const unsigned long long get_current_integer()
        {
            return lexer_context.get_current_integer();
        };

        const char get_current_char_value()
        {
            return lexer_context.get_current_char_value();
        };
    };

    enum class CharacterType : Common::uint8 {
      Letter,
      Unicode,
      Numeric,
      Symbol, 
      Whitespace,
      NewLine,
      EndOfFile,
      Error,
   };

    namespace TypeClassificator {
      inline bool is_neutral_char_type(CharacterType char_type)
      {
         switch (char_type)
         {
         case CharacterType::Whitespace: case CharacterType::NewLine: case CharacterType::EndOfFile:
            return true;
         default:
            return false;
         }
      };

      inline bool is_number_compitable_char_type(CharacterType char_type)
      {
         switch (char_type)
         {
         case CharacterType::Whitespace: case CharacterType::NewLine: case CharacterType::EndOfFile: case CharacterType::Symbol:
            return true;
         default:
            return false;
         }
      };

      inline bool is_numeric_char(char numeric_char)
      {
         return numeric_char >= '0' && numeric_char <= '9';
      };
 
      inline bool is_letter_char(char letter_char)
      {
         return (letter_char >= 'A' && letter_char <= 'Z') || (letter_char >= 'a' && letter_char <= 'z') || letter_char == '_';
      };

      inline bool is_special_char(char special_char)
      {
         return ((special_char >= '!' && special_char <= '~') && !is_numeric_char(special_char) && !is_letter_char(special_char));
      };

      inline bool is_newline_char(char new_line_char)
      {
         return new_line_char == '\n';
      };

      inline bool is_whitespace_char(char whitespace_char)
      {
         return whitespace_char == ' ' || whitespace_char == '\t' || whitespace_char == '\r';
      }; //it was perhaps a mistake that \n is treated as a whitespace instead of a special symbol?

      inline bool is_unicode(char unicode_char)
      {
         return static_cast<unsigned char>(unicode_char) >= 0b10000000;
      };

      inline bool is_hex_code(char hex_code_char)
      {
        return is_numeric_char(hex_code_char) || (hex_code_char >= 'a' && hex_code_char <= 'f') || (hex_code_char >= 'A' && hex_code_char <= 'F');
      };

      inline bool is_bin_code(char bin_code_char)
      {
         return bin_code_char == '0' || bin_code_char == '1';
      };

      inline bool is_valid_char(char unknown_char)
      {
         return (unknown_char >= ' ' && unknown_char <= '~') || is_whitespace_char(unknown_char) || is_unicode(unknown_char) || unknown_char == '\0' || is_newline_char(unknown_char);
      };
   };
}   
