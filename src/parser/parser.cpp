#include <common/language_processing/base.hpp>
#include <common/language_processing/node_base.hpp>
#include <common/language_processing/node_handle.hpp>

#include "parser.hpp"

using namespace AST;

using ParserContext = Common::ParserContext;

//IR LanguageContext(Root)
namespace CLua
{
    //IR ParseFunction(Pattern)
    NodeHandle parse_OptionalWhitespace(ParserContext& context)
    {
        auto start_index = context.source->peeked_char_index;

        //IR QuantityCheck(0,inf,BasePattern(CharRange())) 
        //{min_token, max_token, quantity_expression}

        int min_count = 0;

        while (context.can_consume())
        {
            //IR QuantityExpression(ExpressionSet){expression_set}
            
            //IR QuantityExpression() {expressions}

            //IR IsChar(){is_valid_token}
            auto is_valid = context.see_current() == ' ';
            
            //IsInvalid(is_valid_token)
            if (!is_valid)
            {
                break; //generates break instead of generating order because it's inside of QuantityExpression
            }
            
            context.consume(); //Consumes CharRange pattern
            //ExpressionSet End  

            min_count -= 1;
        }

        //Generated if min_token exists
        if (min_count > 0)
        {
            //IR ErrorEmit(ErrorCode,start_token,end_token) //both start_token and end_token are free expressions
            auto error_node_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_node_handle);

            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedWhitespace);
            error_node.error_span = Common::TokenSpan(start_index,context.source->peeked_char_index);

            return context.record_error(error_node_handle);
        }
        //Generated if max_token exists (max > min & max >= 1)

        //IR ReserveNode() {handle_token, reference_token}
        auto node_handle1 = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto node_handle_ref1 = context.get_node_reference<AST::TokenSpanNode>(node_handle1);

        //IR InitNode(NodeDefinition,reference_token) {node}
        node_handle_ref1.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        node_handle_ref1.token_span = Common::TokenSpan(start_index,context.source->peeked_char_index);
    
        //IR Return(Token) {return_token}
        return node_handle1;
    };

    NodeHandle parse_GenericIdentifier(ParserContext& context)
    {
        auto current_char = context.see_current();
        auto start_index = context.source->peeked_char_index;

        auto is_valid = (current_char >= 'a' && current_char <= 'z') ||
                        (current_char >= 'A' && current_char <= 'Z') || 
                        current_char == '_';

        if (!is_valid)
        {
            auto error_node_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_node_handle);

            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedIdentifier);
            error_node.error_span = Common::TokenSpan(start_index, context.source->peeked_char_index);

            return context.record_error(error_node_handle);
        }

        context.consume(); 
        
        //is_commited can be only switched to true once and only to true

        while (context.can_consume())
        {
            current_char = context.see_current();

            is_valid = (current_char >= 'a' && current_char <= 'z') ||
                    (current_char >= 'A' && current_char <= 'Z') || 
                    current_char == '_' || 
                    (current_char >= '0' && current_char <= '9');

            if (!is_valid)
            {
                break; 
            }
            
            context.consume();
        }
        
        auto node_handle1 = context.reserve_node<Nodes::IdentifierNode>();
        auto node_ref1 = context.get_node_reference<Nodes::IdentifierNode>(node_handle1);

        node_ref1.node_type = static_cast<AST::NodeType>(CLua::NodeType::IdentifierNode);
        node_ref1.span = Common::TokenSpan(start_index,context.source->peeked_char_index);

        return node_handle1;
    }

    NodeHandle parse_CommentContent(ParserContext& context)
    {
        auto start_index = context.source->peeked_char_index;

        while (context.can_consume())
        {
            // Sprawdzamy terminator (NewLineChar: "\n")
            // Ponieważ nie importujemy bibliotek, sprawdzamy to bezpośrednio na kontekście
            auto is_terminator = context.see_current() == '\n';//This only works when a single char needs to be checked
            if (is_terminator)
            {
                // set_inclusive(false) -> zostawiamy kursor PRZED znakiem nowej linii
                break;
            }

            //Optionally (for all patterns or complex patterns)
            /* 
            // Zapisujemy stan na wypadek, gdyby terminator nie spasował 
            // LUB gdyby spasował, ale flaga inclusive była ustawiona na false (exclusive).
            auto terminator_backtrack_state = context.record_cursor();

            // Wywołujemy pełny pod-wzorzec terminatora (może to być dowolnie złożona funkcja)
            auto terminator_res = parse_NewLineChar(context);

            if (!terminator_res.is_error())
            {
                // Sukces! Napotkaliśmy terminator.
                if constexpr (!is_inclusive) // Stała determinowana z TS (.set_inclusive(false))
                {
                    // Jeśli ekskluzywny: przywracamy kursor sprzed parsowania terminatora.
                    // Dzięki temu rodzic będzie mógł go poprawnie skonsumować.
                    context.set_cursor(terminator_backtrack_state);
                }
                // Jeśli inclusive, po prostu zostawiamy kursor tam, gdzie skończył go parsować terminator.
                
                break; 
            }

            // Terminator nie pasuje -> przywracamy stan kursora sprzed próby parsowania terminatora...
            context.set_cursor(terminator_backtrack_state);
            */
            context.consume();
        }

        auto node_handle = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto& node_ref = context.get_node_reference<AST::TokenSpanNode>(node_handle);

        node_ref.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        node_ref.token_span = Common::TokenSpan(start_index, context.source->peeked_char_index);

        return node_handle;
    };

    NodeHandle parse_LineComment(ParserContext& context)
    {
        auto start_index = context.source->peeked_char_index;

        // 1. Dopasowanie CommentStart ("--")
        // Ponieważ wiemy, że to stały dwuznak, generator rozwija go w sekwencję niskopoziomową
        bool match_sequence1 = context.match_sequence("--");

        if (!match_sequence1)
        {
            auto error_node_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_node_handle);

            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedCommentStart); 
            error_node.error_span = Common::TokenSpan(start_index, context.source->peeked_char_index);

            return context.record_error(error_node_handle);
        }

        context.consume(2);

        auto content_res = parse_CommentContent(context);
        if (content_res.is_error())
        {
            return content_res;
        }

        auto has_newline = context.can_consume() && context.see_current() == '\n';
        if (!has_newline)
        {
            // Błąd twardy (commit), bo skonsumowaliśmy już początek komentarza, ale brakuje końca linii
            auto error_node_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_node_handle);

            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedNewline);
            error_node.error_span = Common::TokenSpan(start_index, context.source->peeked_char_index);

            // Commit błędu bezpośrednio na handle przed rejestracją
            error_node_handle.commit(); 
            return context.record_error(error_node_handle);
        }
        context.consume(); // Zjadamy '\n'

        // Sukces: Składamy węzeł AST dla całego LineComment (yields_node)
        auto node_handle = context.reserve_node<CLua::Nodes::Comment>(); // Przypisane mapowanie dla NodeID.Comment
        auto& node_ref = context.get_node_reference<CLua::Nodes::Comment>(node_handle);

        node_ref.node_type = static_cast<AST::NodeType>(CLua::NodeType::Comment);
        // Mapowanie pól do węzła wykonane na poziomie Twojego rejestru IR:
        node_ref.token_span = Common::TokenSpan(start_index, context.source->peeked_char_index);

        return node_handle;
    };

    NodeHandle parse_Expression(ParserContext& context)
    {

    };

    NodeHandle parse_FunctionCall(ParserContext& context)
    {
        auto start_index = context.source->peeked_char_index;
        auto backtrack_state = context.record_cursor();

        auto pattern1 = parse_GenericIdentifier(context);
        if (pattern1.is_error())
        {
            return pattern1;
        }

        auto pattern2 = parse_OptionalWhitespace(context);
        if (pattern2.is_error()) return pattern2;

        if (context.see_current() != '(')
        {
            auto error_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_handle);
            
            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedOpenParen);
            error_node.error_span = Common::TokenSpan(start_index,context.source->peeked_char_index);

            return context.record_error(error_handle);
        }
        context.consume();

        auto pattern3 = parse_OptionalWhitespace(context);
        if (pattern3.is_error())
        {
            return pattern3.commit();
        }

        auto pattern4 = parse_GenericIdentifier(context);
        if (pattern4.is_error())
        {
            return pattern4.commit();
        }

        auto pattern5 = parse_OptionalWhitespace(context);
        if (pattern5.is_error())
        {
            return pattern5.commit();
        }

        if (context.see_current() != ')')
        {
            auto error_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_handle);
            
            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedCloseParen);
            error_node.error_span = Common::TokenSpan(start_index,context.source->peeked_char_index);

            auto final_err = context.record_error(error_handle);

            return final_err.commit();
        }
        context.consume(); 

        auto func_call_handle = context.reserve_node<CLua::Nodes::FuncCallNode>(); 
        auto& func_call_node = context.get_node_reference<CLua::Nodes::FuncCallNode>(func_call_handle);
        
        func_call_node.node_type = static_cast<AST::NodeType>(CLua::NodeType::FuncCallNode);
        func_call_node.identifier = pattern1;

        return func_call_handle;
    }

    NodeHandle parse_StatementParser(ParserContext& context) 
    {
        /*
        export const StatementParser = new P.ChoicePattern()
            .insert_pattern(LocalDeclaration)
            .insert_pattern(FunctionCall)
            .set_pattern_name("StatementParser");

            Strategy of choosing the most right error: (who consumed the furthest index)
        */

        auto backtrack_state = context.record_cursor();
        auto pattern1 = parse_LocalDeclaration(context);

        if (!pattern1.is_error() || pattern1.is_commited_error())
        {
            return pattern1;
        };

        auto error_end_1 = context.source->peeked_char_index;
        NodeHandle output_pattern = pattern1;
        context.set_cursor(backtrack_state);

        auto pattern2 = parse_FunctionCall(context);

        //generated by a naive algorithm
        if (!pattern2.is_error() || pattern2.is_commited_error())
        {
            return pattern2;
        };

        auto error_end_2 = context.source->peeked_char_index;
        if(error_end_2 > error_end_1)
        {
            output_pattern = pattern2;
        };
        //context.set_cursor(backtrack_state); 
        //don't sent the last cursor as useful information is a risk of loss
        //let the parent function decide if to backtrack here.

        return output_pattern;
    }
    /*Naive approach*/
    NodeHandle parse_LocalDeclaration(ParserContext& context)
    {
        auto match_local = context.match_sequence("local");
        auto start_index = context.source->peeked_char_index;

        if(!match_local) 
        {
            auto error_node_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_node_handle);

            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedKeyword);
            error_node.error_span = Common::TokenSpan(start_index,context.source->peeked_char_index);

            return context.record_error(error_node_handle);
        }
        context.consume(5); 

        int min_count = 1;
        while (context.can_consume())
        {
            auto is_valid = context.see_current() == ' ';
            if (!is_valid)
            {
                break;
            }

            min_count -= 1;

            context.consume();
        }
        
        if (min_count > 0)
        {
            auto error_node_handle = context.reserve_node<BaseErrorNode>();
            auto& error_node = context.get_node_reference<BaseErrorNode>(error_node_handle);

            error_node.node_type = static_cast<AST::NodeType>(BaseTypes::Invalid);
            error_node.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedWhitespace);
            error_node.error_span = Common::TokenSpan(start_index,context.source->peeked_char_index);

            return context.record_error(error_node_handle);
        }

        auto pattern1 = parse_GenericIdentifier(context);

        if (pattern1.is_error())
        {
            return pattern1.commit(); 
        }

        auto node_handle1 = context.reserve_node<Nodes::LocalDeclNode>();
        auto node_ref1 = context.get_node_reference<Nodes::LocalDeclNode>(node_handle1);

        node_ref1.node_type = static_cast<AST::NodeType>(CLua::NodeType::LocalDeclNode);
        node_ref1.identifier = pattern1;

        return node_handle1;
    }

    NodeHandle parse_SourceRoot(ParserContext& context)
    {
        
    };

    NodeHandle parse_Root(ParserContext& context)
    {

    };
};

NodeHandle CLua::Parser::generate_AST(ParserContext& context)
{
    return parse_Root(context);
}