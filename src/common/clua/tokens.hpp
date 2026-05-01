#pragma once
#include <common/base.hpp>
#include <common/metacpp.hpp>

namespace CLua {
    enum class TokenType: Common::uint8 {
        Identifier,
        Numeric,
        Symbol,
        Whitespace,
        NewLine,
        Comment,
        String,
        Char,
        EndOfFile,
        Error,
        None
    };

    struct TokenBase
    {
        TokenType token_type = TokenType::None;
        Common::uint64 length = 0;
        Common::uint64 offset = 0;
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
                "Relabeling failed: Derived struct has extra data members"
            );
        
            const TokenType expected = TokenKind<T>::value;

            /*
            static_assert(expected != TokenType::None,
                "Invalid token template type is being used, they must be derived from TokenBase"s
            );
            There's a real use case of NoToken being created from TokenGeneric
            */

            LAssert(
                token_type == expected,
                "expected this token type: " 
                + std::to_string(static_cast<int>(expected)) 
                + " got: " 
                + std::to_string(static_cast<int>(token_type))
            );
            return reinterpret_cast<T&>(*this);
        }
    };
}