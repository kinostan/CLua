#include "parser.hpp"

void match_lua_identifier()
{
    /*
    FirstMatchPattern(
        KeywordPattern("local", TokenType::KeywordLocal),
        KeywordPattern("function", TokenType::KeywordFunction),
        KeywordPattern("if", TokenType::KeywordIf),
        ...
        
        (
            ChoicePattern(RangePattern('a', 'z'), RangePattern('A', 'Z'), Symbol("_")),
            QuantityPattern(0, inf, ChoicePattern(RangePattern('a', 'z'), RangePattern('A', 'Z'), RangePattern('0', '9'), Symbol("_")))
        )
    )]
    */
};
