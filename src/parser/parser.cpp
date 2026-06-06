#include "parser.hpp"

#include <common/clua/tokens.hpp>
#include <common/base.hpp>

namespace CLuaASTParser{
    using namespace CLuaNodes;

    using SymbolKind = CLua::SymbolKind;
    using Keyword = CLua::Keyword;

    using TokenType = CLua::TokenType;
    using NumberBase = CLua::NumberBase;
    using NumberType = CLua::NumberType;
    using Source = CLua::Source;

    NodeHandle get_root_ast_node(ParserContext& parser_context);


    bool is_symbol(ParserContext& parser_context,SymbolKind symbol);
    bool is_keyword(ParserContext& parser_context,Keyword keyword);

    bool expect_symbol(ParserContext& parser_context,SymbolKind symbol);
    bool expect_keyword(ParserContext& parser_context,Keyword keyword);
    bool expect_numeric(ParserContext& parser_context);
    bool expect_string(ParserContext& parser_context);
    bool expect_char(ParserContext& parser_context);
    bool expect_identifier(ParserContext& parser_context);
    

    NodeHandle expect_pattern1(ParserContext& parser_context);
    NodeHandle expect_pattern2(ParserContext& parser_context);
    NodeHandle expect_pattern3(ParserContext& parser_context);
    NodeHandle expect_pattern4(ParserContext& parser_context);
    NodeHandle expect_pattern5(ParserContext& parser_context);
    NodeHandle expect_pattern6(ParserContext& parser_context);
    NodeHandle expect_pattern7(ParserContext& parser_context);

    //[HANDLE_LEXER_ERROR](current_token) --non-critical for parser error but still must be registered and recovered from
    //[EMIT_ERROR] --critical error which requires high level parser to recover

    NodeHandle expect_pattern_with_choice_group_ambigious(ParserContext& parser_context)
    {
        if (!expect_symbol(parser_context,SymbolKind::LBrace))
        {
            //[HANDLE_LEXER_ERROR]
            //[EMIT_ERROR]
        };

        auto current_cursor = parser_context.record_cursor();
        auto error_state_record = parser_context.record_error_state();

        auto choice_output_pattern = InvalidNode;

        //Test pattern1
        choice_output_pattern = expect_pattern1(parser_context);

        if (choice_output_pattern.node_tag == NodeHandleTag::Error) [[likely]]
        {
            parser_context.set_cursor(current_cursor);
            parser_context.set_error_state(error_state_record);
            choice_output_pattern = expect_pattern2(parser_context);
        } 

        if (choice_output_pattern.node_tag == NodeHandleTag::Error) [[likely]]
        {
            parser_context.set_cursor(current_cursor);
            parser_context.set_error_state(error_state_record);
            choice_output_pattern = expect_pattern3(parser_context);
        } 

        if (choice_output_pattern.node_tag == NodeHandleTag::Error) [[unlikely]]
        {
            parser_context.set_cursor(current_cursor);
            parser_context.set_error_state(error_state_record);
            /*
                Since pattern3 was the last in the ChoicePattern element
                then that means the error now must be emitted
            */
            
            //[EMIT_ERROR]
        }


        if (!expect_symbol(parser_context,SymbolKind::RBrace))
        {
            //[HANDLE_LEXER_ERROR](parser_context.see_current_token())
            //[EMIT_ERROR]
        };

        /*
            SomeNodeClass is linked semantically with choice_output_pattern via
            nodes_define.ts node structure definintion file with 
            .insert_field([name of field],MultiChoicePattern) method call

            Because identifying by reference is out of an option then the only
            real way to identify MultiChoicePattern is by defining it's class_name
            via set_pattern_name method
        */
        
        //[CREATE_NODE]<SomeNodeClass>(choice_output_pattern,other_variables)
    };

    NodeHandle expect_pattern_with_choice_group_strict(ParserContext& parser_context)
    {
        /* 
            This pattern is emitted when the pattern itself is resolvable without a deep look up

            Find in nested patterns first primitives keys and test for them.
            For this example let's say that 
            
            Pattern1: started with keyword "let", 
            
            Pattern2: has either number or string token in the beginning (QuantitityPattern or another ChoicePattern)
            
            Pattern3: expects a keyword "fun" in the beginning
        */

        auto current_token = parser_context.see_current_token();

        NodeHandle choice_output_pattern;

        //[HANDLE_LEXER_ERROR]

        switch (current_token.token_type)
        {
        case TokenType::String:
        case TokenType::Numeric:
        {
            choice_output_pattern = expect_pattern2(parser_context);
            break;
        }
        case TokenType::Identifier:
            switch (parser_context.get_current_keyword())
            {
            case Keyword::Let:
                choice_output_pattern = expect_pattern1(parser_context);
                break;
            case Keyword::Fun:
                choice_output_pattern = expect_pattern3(parser_context);
                break;
            default:    
                    //[EMIT_ERROR_INVALID_TOKEN]
            }
        default:
            //[EMIT_ERROR_INVALID_TOKEN]
        }

        //[CREATE_NODE]<SomeNodeClass>(choice_output_pattern,other_variables)    
    };

    NodeHandle expect_pattern_with_choice_group_hybrid(ParserContext& parser_context)
    {
         /* 
            This pattern is partially resolvable without deep look, but then the rest of elements within the list
            require deep look pattern match resolving techniques.
            
            Pattern1: started with keyword "let", 
            
            Pattern2: has either number or string token in the beginning (QuantitityPattern or another ChoicePattern)
            
            Pattern3: expects a keyword "fun" in the beginning

            Pattern4: KW("decl"), KW("metaclua"), Pattern(MetaCLuaDeclaration)

            Pattern5: KW("decl"), KW("metaclua"), Pattern(MetaCLuaExpression)

            Pattern6: KW("decl"), Identifier()
        */
        
        //IRChoicePatternResolve
        NodeHandle choice_output_pattern;
        //IRInDepthChoicePatternAbiguityResolution(choice_output_pattern,ChoicePattern,1))
        auto current_token = parser_context.see_current_token();
        //[HANDLE_LEXER_ERROR](current_token)

        switch (current_token.token_type) //IRTokenChoice
        {
        case TokenType::String:
        case TokenType::Numeric:
        {
            choice_output_pattern = expect_pattern2(parser_context);
            break;
        }
        case TokenType::Identifier:
            {
                switch (parser_context.get_current_keyword()) //IRKeywordChoice
                {
                case Keyword::Let: 
                    //IRKeywordChoice(Let,IRReDefNodeFromCall(choice_output_pattern,pattern1)
                    choice_output_pattern = expect_pattern1(parser_context);
                case Keyword::Fun:
                    //IRKeywordChoice(Fun,IRReDefNodeFromCall(choice_output_pattern,pattern3)
                    choice_output_pattern = expect_pattern3(parser_context);
                case Keyword::Decl: 
                {
                    //IRKeywordChoice(Decl,IRInDepthChoicePatternAmbiguityResolution(choice_output_pattern,ChoicePattern,2))

                    //IRParserStateRecord
                    auto cursor = parser_context.record_cursor();    
                    auto error_state = parser_context.record_error_state();

                    parser_context.get_next_token();//IRAdvanceToken

                    auto current_token2 = parser_context.see_current_token();//IRVarDefineCurrentToken
                    //[HANDLE_LEXER_ERROR](current_token2) 

                    switch (current_token2.token_type)//
                    {
                    case TokenType::Identifier://IRTokenChoice
                        switch (parser_context.get_current_keyword())//IRKeywordChoice
                        {
                        case Keyword::MetaCLua://IRKeywordChoice(MetaCLua,IRChoicePatternAmbigiousInline(choice_output_pattern,ChoicePattern,IRParserStateRecord))
                            //IRChoicePatternAmbigiousInline

                            //IRParserStateSet(IRParserRecord)
                            parser_context.set_cursor(cursor);
                            parser_context.set_error_state(error_state);

                            choice_output_pattern = expect_pattern4(parser_context);

                            if (choice_output_pattern.node_tag == NodeHandleTag::Error)
                            {
                                //IRParserStateSet(IRParserRecord)
                                parser_context.set_cursor(cursor);
                                parser_context.set_error_state(error_state);
                                choice_output_pattern = expect_pattern5(parser_context);
                            };

                            break;
                        case Keyword::Unknown: //it's implicit Identifier type
                            choice_output_pattern = expect_pattern6(parser_context);
                            break;
                        default:
                            //[EMIT_ERROR_INVALID_TOKEN]
                            break;
                        }
                        break;
                    default:
                        //[EMIT_ERROR_INVALID_TOKEN]
                        break;
                    }
                }
                default:    
                    //[EMIT_ERROR_INVALID_TOKEN]
                }
            }
        default:
            //[EMIT_ERROR_INVALID_TOKEN]
        }

        if (choice_output_pattern.node_tag == NodeHandleTag::Error)
        {
            return choice_output_pattern;
        };

        //[CREATE_NODE]<SomeNodeClass>(choice_output_pattern,other_variables)  
    };

    NodeHandle expect_pattern_with_infinite_quantity_pattern(ParserContext& parser_context)
    {
        //IRQuantityPattern(0,inf,Pattern7) 

        //IRParserStateRecord
        auto cursor = parser_context.record_cursor();    
        auto error_state = parser_context.record_error_state();

        //IROptionalPattern
        auto head_node = expect_pattern7(parser_context);
        if (head_node.node_tag == NodeHandleTag::Error)
        {

            //IRParserStateSet(IRParserStateRecord,min)
            //when min == 0
            parser_context.set_cursor(cursor);
            parser_context.set_error_state(error_state);

            return NodeHandle(NodeHandleTag::NoPattern,0);

            //when min >= 1 
            return head_node;
        };

        auto current_bottom = head_node;
    
        while (!parser_context.has_reached_end())
        {
            //IRParserStateRecord
            auto cursor = parser_context.record_cursor();    
            auto error_state = parser_context.record_error_state();

            auto next_bottom = expect_pattern7(parser_context);

            if (next_bottom.node_tag == NodeHandleTag::Error)
            {
                //IRParserStateSet(IRParserStateRecord)
                parser_context.set_cursor(cursor);
                parser_context.set_error_state(error_state);
                break;
            };

            //auto& current_bottom_ref = parser_context.get_node_from_handle<HeadNodeType>(current_bottom);
            //current_bottom_ref.[GET_NEXT_ELEMENT_FIELD_NAME] = next_bottom;

            current_bottom = next_bottom;
        }

    };
    
    NodeHandle Parser::generate_AST(){
        return get_root_ast_node(parser_context);
    }
}