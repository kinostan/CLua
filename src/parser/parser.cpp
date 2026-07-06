#include <common/language_processing/node_handle.hpp>

#include "parser.hpp"

using namespace AST;

using ParserContext = Common::ParserContext;

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
};

NodeHandle CLua::Parser::generate_AST(ParserContext& parser_context)
{
    return root(parser_context);    
}