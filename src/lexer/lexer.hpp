#pragma once

#include <debugger/debugger.hpp>
#include <metadata/symbol_classifier.hpp>
#include <metadata/keyword_classifier.hpp>

#include <stdint.h>
#include <vector>
#include <type_traits>
#include <concepts>

namespace Util {

    using namespace std::string_literals;

    enum class ConsumerMode: uint8_t  {
        CLua,
        MetaCLua
    };

    enum class MetaConsumerMode: uint8_t {
        None,        
        Meta,
        LuaCapture,
        LuaEmbed
    };

    enum class ErrorCode: uint8_t {
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
        UnclosedLuaBlock,
    };

    enum class TokenType: uint8_t {
        Identifier,
        Numeric,
        Symbol,
        Whitespace,
        NewLine,
        Comment,
        String,
        Char,
        EndOfFile,
        LuaBlock,
        Error,
        None
    };

    enum class NumberType: uint8_t {
        Integer,
        Float,
        None,
    };

    enum class NumberBase: uint8_t  {
        Hexdecimal,
        Decimal,
        Binary,
        None,
    };

    struct SourceView {
        unsigned char* source_buffer;
        size_t source_size;
    };

    class Source {
        public:
        size_t index;
        size_t source_size;    
        private:   
        unsigned char* source_buffer;

        public:
        
        Source() = default;
        Source(unsigned char* source_buffer, size_t source_size) : source_buffer(source_buffer), source_size(source_size), index(0)
        {
            LAssert(source_buffer,
                "Source buffer must exist"s
            );
        };

        std::string_view slice_string(size_t string_index, size_t string_length)
        {
            return std::string_view(
                reinterpret_cast<char*>(source_buffer + string_index),
                string_length
            );
        };

        Source slice(size_t start_index,size_t length)
        {
            LAssert(length > 0,
                "length must be greater than 0"s
            )
            size_t end_index = start_index + length;
            LAssert(
                end_index <= source_size,
                "broken assumption that end_index <= source_size is true"s
            )
            return Source(source_buffer + start_index,length);
        };

        Source slice(size_t start_index)
        {
            LAssert(
                source_size > start_index,
                "source_size > start_index is not true"s
            )

            return slice(start_index,source_size-start_index);
        };

        inline unsigned char* get_source_buffer()
        {
            return source_buffer;
        };

        inline bool can_consume_sentinel(size_t consume_distance = 1)
        {
            //source_size, because the additional character is a null terminator
            return index + consume_distance - 1 < source_size + 1;
        };

        inline bool can_consume(size_t consume_distance = 1)
        {
            return index + consume_distance - 1 < source_size;
        };

        inline void consume(size_t consume_distance = 1)
        {
            LAssert(
                can_consume_sentinel(consume_distance),
                "index is reading beyond the source_buffer"s
            );
            index += consume_distance;
        };

        inline unsigned char see_current()
        {
            LAssert(
                can_consume_sentinel(),
                "index is reading beyond the source_buffer"s
            );
            if (!can_consume())
            {
                return '\0';
            };
            return source_buffer[index];
        };

        inline bool can_peek_sentinel(size_t peek_distance) const noexcept
        {  
            return (index+peek_distance) < source_size + 1;
        };

        inline bool can_peek(size_t peek_distance = 1) const noexcept
        {
            return (index+peek_distance) < source_size;
        };

        inline unsigned char peek(size_t peek_distance = 1)
        {   
            LAssert(
                can_peek_sentinel(peek_distance),
                "Can't peek here"s
            );
            if (!can_peek())
            {
                return (unsigned char)'\0';
            };
            return source_buffer[index+peek_distance];
        };
    
        inline void set_index(size_t new_index)
        {
            LAssert(new_index < source_size, 
                "new_index is stepping outside of source buffer"s
            )
            index = new_index;
        };
    };

    struct TokenBase
    {
        TokenType token_type = TokenType::None;
        size_t length = 0;
        size_t offset = 0;
    };
    template <typename T>
    struct TokenKind {
        inline static constexpr TokenType value = TokenType::None;
    };

    struct IdentifierToken : TokenBase {};
    template<>
    struct TokenKind<IdentifierToken> {
        inline static constexpr TokenType value = TokenType::Identifier;
    };  

    struct NumericToken : TokenBase {};
    template<>
    struct TokenKind<NumericToken> {
        inline static constexpr TokenType value = TokenType::Numeric;
    };  

    struct SymbolToken : TokenBase {};
    template<>
    struct TokenKind<SymbolToken> {
        inline static constexpr TokenType value = TokenType::Symbol;
    };  

    struct WhitespaceToken : TokenBase {};
    template<>
    struct TokenKind<WhitespaceToken> {
        inline static constexpr TokenType value = TokenType::Whitespace;
    };  

    struct CommentToken: TokenBase {};
    template<>
    struct TokenKind<CommentToken>
    {
        inline static constexpr TokenType value = TokenType::Comment;
    };

    struct StringToken : TokenBase {};
    template<>
    struct TokenKind<StringToken> {
        inline static constexpr TokenType value = TokenType::String;
    };  

    struct CharToken: TokenBase {};
    template<>
    struct TokenKind<CharToken> {
        inline static constexpr TokenType value = TokenType::Char;
    };

    struct NewLineToken : TokenBase {};
    template<>
    struct TokenKind<NewLineToken> {
        inline static constexpr TokenType value = TokenType::NewLine;
    };  

    struct LuaBlockToken: TokenBase {};
    template<>
    struct TokenKind<LuaBlockToken>
    {
        inline static constexpr TokenType value = TokenType::LuaBlock;
    };

    struct EOFToken : TokenBase {};
    template<>
    struct TokenKind<EOFToken> {
        inline static constexpr TokenType value = TokenType::EndOfFile;
    };  

    struct ErrorToken : TokenBase {};   
    template<>
    struct TokenKind<ErrorToken> {
        inline static constexpr TokenType value = TokenType::Error;
    };  

    struct NoToken : TokenBase {};   
    template<>
    struct TokenKind<NoToken> {
        inline static constexpr TokenType value = TokenType::None;
    };  

    struct TokenGeneric : TokenBase {
        template <typename T>
        requires std::derived_from<T, TokenBase>
        T& as() {
            static_assert(sizeof(T) == sizeof(TokenGeneric), 
                "Relabeling failed: Derived struct has extra data members"s);
        
            const TokenType expected = TokenKind<T>::value;

            /*
            static_assert(expected != TokenType::None,
                "Invalid token template type is being used, they must be derived from TokenBase"s
            );
            There's a real use case of NoToken being created from TokenGeneric
            */

            LAssert(
                token_type == expected,
                LexerError 
                + "expected this token type: "s 
                + std::to_string(static_cast<int>(expected)) 
                + " got: "s 
                + std::to_string(static_cast<int>(token_type)) 
                + LexerErrorEnd
            );

            return reinterpret_cast<T&>(*this);
        }
    };

    struct Error {
        ErrorCode error_code;
    };

    struct NumberHint {
        NumberType number_type = NumberType::None;
        NumberBase number_base = NumberBase::None;
    };

    struct LuaUCaptureState {
        size_t brace_balance = 0; //Brace balance is how many "[" braces are against "]"
        bool met_first_brace = false;
    };

    struct LuaUCodeState {
        size_t brace_balance = 0; //Brace balance is how many "{" braces are against "}"
        bool met_first_brace = false;
    };
    
    class LexerContext {
        private:
        bool emitted = false;
        ConsumerMode consumer_mode = ConsumerMode::CLua;
        MetaConsumerMode meta_consumer_mode = MetaConsumerMode::None;
        public:

        LuaUCaptureState luau_capture_state;
        LuaUCodeState luau_code_state;

        Source source;

        Error last_error;
        NumberHint last_number;
        SymbolClassifier::SymbolKind last_symbol = SymbolClassifier::SymbolKind::Unknown;  
        KeywordClassifier::Keyword last_keyword = KeywordClassifier::Keyword::Unknown;

        uint64_t last_number_integer = 0;
        long double last_number_fraction = 0; //belongs to <0,inf) in any other case it's invalid

        TokenType ultimate_token_type = TokenType::Error;
        TokenType original_token_type = ultimate_token_type; //this variable is strictly for recover if user chooses to do so

        LexerContext() = default;
        LexerContext(Source& source): source(source)
        {};

        inline ConsumerMode see_current_consumer_mode()
        {
            return consumer_mode;
        };

        inline void switch_consumer_mode(ConsumerMode new_consumer_mode)
        {
            if (new_consumer_mode != ConsumerMode::MetaCLua)
            {
                switch_meta_consumer_mode(MetaConsumerMode::None);
            } else
            {
                switch_meta_consumer_mode(MetaConsumerMode::Meta);
            };
            consumer_mode = new_consumer_mode;
            luau_capture_state = LuaUCaptureState();
            luau_code_state = LuaUCodeState();
        };

        inline MetaConsumerMode see_current_meta_consumer_mode()
        {
            return meta_consumer_mode;
        };

        inline void switch_meta_consumer_mode(MetaConsumerMode new_meta_consumer_mode)
        {
            meta_consumer_mode = new_meta_consumer_mode;
        };

        inline void token_enter()
        {
            emitted = false;
        };

        private:
        inline void on_emit(){
            LAssert(
                !emitted,
                "trying to emit hint multiple times within the same token"s
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
            last_error = error;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<ErrorToken>::value;
        };

        inline void record_number(NumberBase number_base, NumberType number_type, uint64_t number_integer,long double number_fraction = 0)
        {
            on_emit();

            LAssert(
                last_number_fraction >= 0,
                "lexer can't consume unary minus operator"s
            );

            last_number_integer = number_integer;
            last_number_fraction = number_fraction;

            NumberHint number_hint;
            number_hint.number_base = number_base;
            number_hint.number_type = number_type;

            last_number = number_hint;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<NumericToken>::value;
        };

        inline void record_symbol(SymbolClassifier::SymbolKind symbol)
        {
            on_emit();

            last_symbol = symbol;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<SymbolToken>::value;
        };

        inline void record_identifier(std::string_view identifier)
        {
            on_emit();

            auto keyword_type = KeywordClassifier::get_keyword_type(identifier);
            last_keyword = keyword_type;

            original_token_type = ultimate_token_type;
            ultimate_token_type = TokenKind<IdentifierToken>::value;
        };
    };

    class Lexer
    {
        private:
        LexerContext lexer_context;
        TokenGeneric last_peeked_token = TokenGeneric();

        public:
        Lexer() = default;
        Lexer(Util::Source& source)
        {
            lexer_context = LexerContext(source);
        };

        private:
        TokenGeneric get_next_token();
        
        inline void rollback_cursor()
        {
            lexer_context.source.set_index(last_peeked_token.offset);
        };

        public:
        
        LexerContext& get_lexer_context()
        {
            return lexer_context;
        };

        TokenGeneric process_next_token()
        {
            if (last_peeked_token.token_type != TokenType::None)
            {
                auto last_token_copy = last_peeked_token;
                last_peeked_token.as<NoToken>();

                return last_token_copy;
            }
            lexer_context.token_enter();
            return get_next_token();
        };

        TokenGeneric peek_next_token() {
            LAssert(
                last_peeked_token.token_type == TokenType::None, 
                "can't peek next token again after doing it before"s
            )
            lexer_context.token_enter();
            auto token = get_next_token();
            last_peeked_token = token;            
            return token;
        }

        const Error get_last_error()
        {
            return lexer_context.last_error;
        };

        const NumberHint get_last_number_hint()
        {
            return lexer_context.last_number;
        };

        const SymbolClassifier::SymbolKind get_last_symbol()
        {
            return lexer_context.last_symbol;
        };

        const KeywordClassifier::Keyword get_last_keyword()
        {
            return lexer_context.last_keyword;
        };
    };

    enum class CharacterType : uint8_t {
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

      inline bool is_number_compapitable_char_type(CharacterType char_type)
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
