#include <common/language_processing/base.hpp>
#include <common/language_processing/node_base.hpp>
#include <common/language_processing/node_handle.hpp>

using namespace AST;

#include "parser.hpp"
using ParserContext = Common::ParserContext;

namespace CLua
{
    NodeHandle parse_OptionalWhitespace(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        auto handle_token = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto& reference_token = context.get_node_reference<AST::TokenSpanNode>(handle_token);
        while (context.can_consume() && context.see_current() == ' ') context.consume();
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        reference_token.token_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_RequiredWhitespace(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        auto handle_token = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto& reference_token = context.get_node_reference<AST::TokenSpanNode>(handle_token);
        int min_token = 1;
        while (context.can_consume() && context.see_current() == ' ') { context.consume(); --min_token; }
        if (min_token > 0) {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedWhitespace);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        reference_token.token_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_GenericIdentifier(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        auto is_valid_token = context.can_consume() && ((context.see_current() >= 'a' && context.see_current() <= 'z') || (context.see_current() >= 'A' && context.see_current() <= 'Z') || context.see_current() == '_');
        if (!is_valid_token) {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedIdentifier);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        context.consume();
        while (context.can_consume()) {
            auto current_token = context.see_current();
            is_valid_token = (current_token >= 'a' && current_token <= 'z') || (current_token >= 'A' && current_token <= 'Z') || current_token == '_' || (current_token >= '0' && current_token <= '9');
            if (!is_valid_token) break;
            context.consume();
        }
        auto handle_token = context.reserve_node<Nodes::IdentifierNode>();
        auto& reference_token = context.get_node_reference<Nodes::IdentifierNode>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(CLua::NodeType::IdentifierNode);
        reference_token.span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_CommentContent(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        while (context.can_consume() && context.see_current() != '\n') context.consume();
        auto handle_token = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto& reference_token = context.get_node_reference<AST::TokenSpanNode>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        reference_token.token_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_LineComment(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        auto whitespace_token = parse_OptionalWhitespace(context);
        if (!context.match_sequence("--")) {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedCommentStart);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = whitespace_token;
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        context.consume(2);
        auto content_token = parse_CommentContent(context);
        if (!context.can_consume() || context.see_current() != '\n') {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedNewline);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = content_token;
            auto result_token = context.record_error(error_handle_token,CLua::LanguageID);
            return result_token.commit();
        }
        context.consume();
        auto handle_token = context.reserve_node<Nodes::Comment>();
        auto& reference_token = context.get_node_reference<Nodes::Comment>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(CLua::NodeType::Comment);
        reference_token.token_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_StringContent(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        while (context.can_consume() && context.see_current() != '"') context.consume();
        auto handle_token = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto& reference_token = context.get_node_reference<AST::TokenSpanNode>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        reference_token.token_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_StringLiteral(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        if (!context.can_consume() || context.see_current() != '"') {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedStringClosure);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        context.consume();
        auto content_token = parse_StringContent(context);
        if (!context.can_consume() || context.see_current() != '"') {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedStringClosure);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = content_token;
            auto result_token = context.record_error(error_handle_token,CLua::LanguageID);
            return result_token.commit();
        }
        context.consume();
        return content_token;
    }

    NodeHandle parse_NumberLiteral(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        auto handle_token = context.reserve_compiler_node<AST::TokenSpanNode>();
        auto& reference_token = context.get_node_reference<AST::TokenSpanNode>(handle_token);
        int min_token = 1;
        int max_token = 10;
        while (context.can_consume() && max_token > 0 && context.see_current() >= '0' && context.see_current() <= '9') { context.consume(); --min_token; --max_token; }
        if (min_token > 0) {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::Unspecified);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);
        } else if (max_token < 0)
        {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::Unspecified);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);            
        }
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::TokenSpan);
        reference_token.token_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
        return handle_token;
    }

    NodeHandle parse_Expression(ParserContext& context)
    {
        parse_OptionalWhitespace(context);
        auto backtrack_state_token = context.record_cursor();
        auto string_token = parse_StringLiteral(context);
        if (!string_token.is_error() || string_token.is_commited_error()) return string_token;
        auto string_end_token = context.source->peeked_char_index;
        context.set_cursor(backtrack_state_token);
        auto number_token = parse_NumberLiteral(context);
        if (!number_token.is_error() || number_token.is_commited_error()) return number_token;
        auto number_end_token = context.source->peeked_char_index;
        context.set_cursor(backtrack_state_token);
        auto identifier_token = parse_GenericIdentifier(context);
        if (!identifier_token.is_error() || identifier_token.is_commited_error()) return identifier_token;
        auto identifier_end_token = context.source->peeked_char_index;
        if (string_end_token > number_end_token && string_end_token > identifier_end_token) { context.set_cursor(backtrack_state_token); return parse_StringLiteral(context); }
        if (number_end_token > identifier_end_token) { context.set_cursor(backtrack_state_token); return parse_NumberLiteral(context); }
        return identifier_token;
    }

    NodeHandle parse_ArgumentList(ParserContext& context)
    {
        auto handle_token = context.reserve_compiler_node<AST::LinkedNodeList>();
        auto& reference_token = context.get_node_reference<AST::LinkedNodeList>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::LinkedList);
        reference_token.value = context.get_null_node();
        reference_token.next = context.get_null_node();
        auto current_tail_token = handle_token;
        auto previous_backtrack_state_token = context.record_cursor();
        while (true) {
            auto expression_token = parse_Expression(context);
            if (expression_token.is_error()) {
                if (expression_token.is_commited_error()) return expression_token;
                context.set_cursor(previous_backtrack_state_token);
                break;
            }
            auto& current_tail_reference_token = context.get_node_reference<AST::LinkedNodeList>(current_tail_token);
            if (current_tail_reference_token.value == context.get_null_node()) current_tail_reference_token.value = expression_token;
            else {
                auto next_handle_token = context.reserve_compiler_node<AST::LinkedNodeList>();
                auto& next_reference_token = context.get_node_reference<AST::LinkedNodeList>(next_handle_token);
                next_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::LinkedList);
                next_reference_token.value = expression_token;
                next_reference_token.next = context.get_null_node();
                current_tail_reference_token.next = next_handle_token;
                current_tail_token = next_handle_token;
            }
            previous_backtrack_state_token = context.record_cursor();
            if (!context.can_consume() || context.see_current() != ',') break;
            context.consume();
        }
        if (reference_token.value == context.get_null_node()) {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedIdentifier);
            error_reference_token.error_span = Common::TokenSpan(context.source->peeked_char_index, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        return handle_token;
    }

    NodeHandle parse_FunctionCall(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        auto identifier_token = parse_GenericIdentifier(context);
        if (identifier_token.is_error()) return identifier_token;
        parse_OptionalWhitespace(context);
        if (!context.can_consume() || context.see_current() != '(') {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedOpenParen);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = identifier_token;
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        context.consume();
        auto arguments_token = parse_ArgumentList(context);
        if (arguments_token.is_error()) return arguments_token.commit();
        if (!context.can_consume() || context.see_current() != ')') {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedCloseParen);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = arguments_token;
            auto result_token = context.record_error(error_handle_token,CLua::LanguageID);
            return result_token.commit();
        }
        context.consume();
        auto handle_token = context.reserve_node<Nodes::FuncCallNode>();
        auto& reference_token = context.get_node_reference<Nodes::FuncCallNode>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(CLua::NodeType::FuncCallNode);
        reference_token.identifier = identifier_token;
        return handle_token;
    }

    NodeHandle parse_LocalDeclaration(ParserContext& context)
    {
        auto start_token = context.source->peeked_char_index;
        if (!context.match_sequence("local")) {
            auto error_handle_token = context.reserve_node<AST::BaseErrorNode>();
            auto& error_reference_token = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
            error_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
            error_reference_token.error_code = static_cast<AST::ErrorCode>(CLua::ErrorCode::ExpectedKeyword);
            error_reference_token.error_span = Common::TokenSpan(start_token, context.source->peeked_char_index);
            error_reference_token.ast_byproduct = context.get_null_node();
            return context.record_error(error_handle_token,CLua::LanguageID);
        }
        context.consume(5);
        auto whitespace_token = parse_RequiredWhitespace(context);
        if (whitespace_token.is_error()) return whitespace_token.commit();
        auto identifier_token = parse_GenericIdentifier(context);
        if (identifier_token.is_error()) return identifier_token.commit();
        auto handle_token = context.reserve_node<Nodes::LocalDeclNode>();
        auto& reference_token = context.get_node_reference<Nodes::LocalDeclNode>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(CLua::NodeType::LocalDeclNode);
        reference_token.identifier = identifier_token;
        return handle_token;
    }

    NodeHandle parse_StatementParser(ParserContext& context)
    {
        auto backtrack_state_token = context.record_cursor();
        auto local_token = parse_LocalDeclaration(context);
        if (!local_token.is_error() || local_token.is_commited_error()) return local_token;
        auto local_end_token = context.source->peeked_char_index;
        context.set_cursor(backtrack_state_token);
        auto call_token = parse_FunctionCall(context);
        if (!call_token.is_error() || call_token.is_commited_error()) return call_token;
        auto call_end_token = context.source->peeked_char_index;
        if (local_end_token > call_end_token) { context.set_cursor(backtrack_state_token); return parse_LocalDeclaration(context); }
        return call_token;
    }

    NodeHandle parse_SourceRoot(ParserContext& context)
    {
        auto handle_token = context.reserve_compiler_node<AST::LinkedNodeList>();
        auto& reference_token = context.get_node_reference<AST::LinkedNodeList>(handle_token);
        reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::LinkedList);
        reference_token.value = context.get_null_node();
        reference_token.next = context.get_null_node();
        auto current_tail_token = handle_token;
        auto previous_backtrack_state_token = context.record_cursor();
        while (context.can_consume()) {
            auto choice_backtrack_state_token = context.record_cursor();
            auto unit_token = parse_LineComment(context);
            if (unit_token.is_error() && !unit_token.is_commited_error()) { context.set_cursor(choice_backtrack_state_token); unit_token = parse_StatementParser(context); }
            if (unit_token.is_error()) {
                if (unit_token.is_commited_error()) return unit_token;
                context.set_cursor(previous_backtrack_state_token);
                break;
            }
            auto& current_tail_reference_token = context.get_node_reference<AST::LinkedNodeList>(current_tail_token);
            if (current_tail_reference_token.value == context.get_null_node()) current_tail_reference_token.value = unit_token;
            else {
                auto next_handle_token = context.reserve_compiler_node<AST::LinkedNodeList>();
                auto& next_reference_token = context.get_node_reference<AST::LinkedNodeList>(next_handle_token);
                next_reference_token.node_type = static_cast<AST::NodeType>(AST::BaseTypes::LinkedList);
                next_reference_token.value = unit_token;
                next_reference_token.next = context.get_null_node();
                current_tail_reference_token.next = next_handle_token;
                current_tail_token = next_handle_token;
            }
            previous_backtrack_state_token = context.record_cursor();
        }
        return handle_token;
    }

    NodeHandle parse_Root(ParserContext& context)
    {
        return parse_SourceRoot(context);
    }
}

NodeHandle CLua::Parser::generate_AST(ParserContext& context)
{
    return CLua::parse_Root(context);
}