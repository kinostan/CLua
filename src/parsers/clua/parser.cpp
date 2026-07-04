#include <common/language_processing/node_handle.hpp>

#include "parser.hpp"

using namespace AST;
using namespace CLua;

using ParserContext = Common::Parser::ParserContext;
using TokenType = Common::Lexer::TokenType;
using TokenSpan = Common::Lexer::TokenSpan;

NodeHandle parse_optional_whitespace(ParserContext& parser_context)
{
    auto current_token = parser_context.see_current_token();

    while (current_token.token_type == TokenType::Whitespace)
    {
        parser_context.advance_token();
        parser_context.peek_next_token();
        current_token = parser_context.see_current_token();        
    };
};

CLua::Keyword parse_keyword(ParserContext& parser_context)
{
    //generated from 
    auto is_if = parser_context.match_word("if");
    auto continues = parser_context.match_symbols("_",2);

    if (is_if && !continues)
    {
        return Keyword::If;
    };

    return Keyword::Unknown;
};

NodeHandle parse_if_statement(ParserContext& parser_context)
{

}

NodeHandle root(ParserContext& parser_context)
{
    /* 
        All patterns have optional whitespace on their left side implicitly
        
        //Expression for simplicity is for now only an identifier
        //Definition of identifier:
        Identifier = new Pattern("Identifier")
        .insert_pattern(
            new ChoicePattern()
            .insert_pattern(new Symbol("_"))
            .insert_pattern(new Word())
        )
        .insert_pattern(
            new QuantityPattern(
                new ChoicePattern()
                .insert_pattern(new Symbol("_"))
                .insert_pattern(new Word())
                .insert_pattern(new Number())
            )
        );

        Root = new Pattern("Root")
        .insert_pattern(
            new ChoicePattern()
            .insert_pattern(IfStatement) 
            .insert_pattern(Expression)
        )
        .insert_pattern(EOF); --< EOF pattern with implicit whitespace to consume on the left
    */

    //guess keyword (expecting if first)
    //this says false if word still continues
    //function match_word has a lot of presumptions
    //1. it's synchronized in the exact token between parser and lexer
    //2. match_word explicitly is expecting a word token 
    //3. match_word emits true if word token has the same length as the given word
    //   and the next character isn't the of word type
    //4. implication is as such that hence identifier has optional number or symbol
    //   to fully disambiguate what type of token I am seeing I must look up next token
    //   and only then confirm it's non-identifier token.

    auto token_span_product = TokenSpan();

    if (parser_context.match_word("if")) 
    {
        //guess was a partial success now

        //here's a problem though with how to efficiently express that I want to look up 
        //another token, but not lex it fully
        //(helper function needs to be added to parser's API or make it fully integrated into lexer)
        //(optionally the idea of lexer is fully removed and lexer gets fully integrated into parser)
        //the result would be that parser owns source and manages it and there are not high level 
        //abstractions like token types 
        //(although they would exist in the sense that there are keywords and some special tokens like EOF)
        //But there is no diambiguation between symbol or character. 
        //The implication of such decision is that dismabiguation of patterns happens at per character level
        //this allows high specialization, but also potentially large code emittions which are harder to debug

        parser_context.peek_next_token();
    };
};

NodeHandle CLuaParser::generate_AST(ParserContext& parser_context)
{
    parser_context.peek_next_token();
    return root(parser_context);    
}