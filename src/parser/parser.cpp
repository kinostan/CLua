#include "parser.hpp"

#include <common/clua/tokens.hpp>
#include <common/base.hpp>

namespace CLuaASTParser{
    using namespace CLuaNodes;

    using SymbolKind = CLua::SymbolKind;

    using TokenType = CLua::TokenType;
    using NumberBase = CLua::NumberBase;
    using NumberType = CLua::NumberType;
    using Source = CLua::Source;

    NodeHandle get_root_ast_node(ParserContext& parser_context);

    NodeHandle expect_pattern_with_choice_group(ParserContext& parser_context)
    {
        if (!expect_symbol(SymbolKind::LBrace))
        {
            [EMIT_ERROR];
        };
        [HANDLE_LEXER_ERROR];

        auto current_cursor = parser_context.record_cursor();
        auto error_state_record = parser_context.record_error_state();
        //include error list length and the errors
        //also switch off node error emittion or create explicit mechanism
        //for error emittion or just wait for confirmation maybe?

        auto choice_output_pattern = InvalidNode;

        //Test pattern1
        choice_output_pattern = expect_pattern1(parser_context);

        if (choice_output_pattern.node_tag == NodeHandleTag::Error)
        {
            parser_context.set_cursor(current_cursor);
            parser_context.set_error_state(error_state_record);
            choice_output_pattern = expect_pattern2(parser_context);
        } 

        if (choice_output_pattern.node_tag == NodeHandleTag::Error)
        {
            parser_context.set_cursor(current_cursor);
            parser_context.set_error_state(error_state_record);
            choice_output_pattern = expect_pattern3(parser_context);
        } 

        if (choice_output_pattern.node_tag == NodeHandleTag::Error)
        {
            parser_context.set_cursor(current_cursor);
            parser_context.set_error_state(error_state_record);
            /* 
                Since pattern3 was the last in the ChoicePattern element
                then that means the error now must be emitted
            */
            [EMIT_ERROR]
        }

        /*
        other patterns
        */

        if (!expect_symbol(SymbolKind::RBrace))
        {
            [EMIT_ERROR];
        };
        [HANDLE_LEXER_ERROR];

        /* 
            SomeNodeClass is linked semantically with choice_output_pattern via
            nodes_define.ts node structure definintion file with 
            .insert_field([name of field],MultiChoicePattern) method call

            Because identifying by reference is out of an option then the only
            real way to identify MultiChoicePattern is by defining it's class_name
            via set_pattern_name method
        */
        return parser_context.create_node<SomeNodeClass>(choice_output_pattern,some_other_varaibles);
    };

    NodeHandle Parser::generate_AST(){
        return get_root_ast_node(parser_context);
    }
}