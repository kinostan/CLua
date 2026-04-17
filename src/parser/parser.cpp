#include "parser.hpp"

namespace ASTParser{
    template <typename T, typename... Ts>
    constexpr bool is_one_of(const T& value, Ts... values)
    {   
        return ((value == values) || ...); // fold expression (C++17+)
    }

    using namespace CLuaNodes;
    using namespace SymbolClassifier;

    using TokenType = Util::TokenType;
    using NumberBase = Util::NumberBase;
    using NumberType = Util::NumberType;
    using Source = Util::Source;

    namespace Expression {
        namespace {
            void ensure_current_token(ParserContext& parser_context)
            {
                if (parser_context.see_current_token().token_type == TokenType::None)
                {
                    parser_context.get_next_token();
                }
            }

            CLuaNodes::NodeHandle emit_syntax_error(ParserContext& parser_context, CLuaNodes::NodeType error_node_type = CLuaNodes::NodeType::Invalid)
            {
                ParserError parser_error;
                parser_error.error_node_type = error_node_type;
                return parser_context.emit_error(parser_error);
            }

            bool is_identifier_path_separator(SymbolKind symbol_kind)
            {
                return symbol_kind == SymbolKind::Dot || symbol_kind == SymbolKind::DoubleColon;
            }

            IdentifierPathSeparator to_identifier_path_separator(SymbolKind symbol_kind)
            {
                switch (symbol_kind)
                {
                case SymbolKind::Dot:
                    return IdentifierPathSeparator::Dot;
                case SymbolKind::DoubleColon:
                    return IdentifierPathSeparator::DoubleColon;
                default:
                    PAssert(false, "symbol is not a valid identifier path separator");
                    return IdentifierPathSeparator::Dot;
                }
            }

            NodeHandle process_identifier_or_path(ParserContext& parser_context)
            {
                auto root_token = parser_context.see_current_token();

                auto root_handle = parser_context.create_node<Identifier>();
                auto& root_node = parser_context.get_node_from_handle<Identifier>(root_handle);
                root_node.identifier_token = root_token;

                parser_context.get_next_token();

                NodeHandle chain_head = InvalidNode;
                NodeHandle chain_tail = InvalidNode;

                auto append_path_segment = [&](const Util::TokenGeneric& identifier_token, IdentifierPathSeparator separator) {
                    auto segment_handle = parser_context.create_node<IdentifierPathNode>();
                    auto& segment_node = parser_context.get_node_from_handle<IdentifierPathNode>(segment_handle);
                    segment_node.identifier_token = identifier_token;
                    segment_node.separator_from_previous = separator;
                    segment_node.next_segment = InvalidNode;

                    if (chain_head == InvalidNode)
                    {
                        chain_head = segment_handle;
                        chain_tail = segment_handle;
                        return;
                    }

                    auto& tail_node = parser_context.get_node_from_handle<IdentifierPathNode>(chain_tail);
                    tail_node.next_segment = segment_handle;
                    chain_tail = segment_handle;
                };

                bool has_path = false;

                while (parser_context.see_current_token().token_type == TokenType::Symbol &&
                    is_identifier_path_separator(parser_context.get_current_symbol()))
                {
                    has_path = true;
                    auto separator = to_identifier_path_separator(parser_context.get_current_symbol());
                    parser_context.get_next_token();

                    if (parser_context.see_current_token().token_type != TokenType::Identifier)
                    {
                        return emit_syntax_error(parser_context);
                    }

                    append_path_segment(parser_context.see_current_token(), separator);

                    parser_context.get_next_token();
                }

                if (!has_path)
                {
                    return root_handle;
                }

                auto path_head_handle = parser_context.create_node<IdentifierPathNode>();
                auto& path_head_node = parser_context.get_node_from_handle<IdentifierPathNode>(path_head_handle);
                path_head_node.identifier_token = root_token;
                path_head_node.separator_from_previous = IdentifierPathSeparator::None;
                path_head_node.next_segment = chain_head;

                return path_head_handle;
            }
        }

        NodeHandle process_atom(ParserContext& parser_context)
        {
            ensure_current_token(parser_context);

            auto current_token = parser_context.see_current_token();

            if (current_token.token_type == TokenType::Symbol &&
                parser_context.get_current_symbol() == SymbolKind::LParen)
            {
                parser_context.get_next_token();

                auto grouped_expression = process_atom(parser_context);

                current_token = parser_context.see_current_token();
                if (current_token.token_type != TokenType::Symbol ||
                    parser_context.get_current_symbol() != SymbolKind::RParen)
                {
                    return emit_syntax_error(parser_context);
                }

                parser_context.get_next_token();
                return grouped_expression;
            }

            switch (current_token.token_type)
            {
            case TokenType::Numeric: {
                auto number_hint = parser_context.get_current_number_hint();
                auto& lexer_context = parser_context.get_lexer().get_lexer_context();

                if (number_hint.number_type == NumberType::Integer)
                {
                    auto node_handle = parser_context.create_node<CLuaNodes::IntegerLiteral>();
                    auto& node = parser_context.get_node_from_handle<CLuaNodes::IntegerLiteral>(node_handle);
                    node.value = static_cast<long long>(lexer_context.current_number_integer);
                    parser_context.get_next_token();
                    return node_handle;
                }

                auto node_handle = parser_context.create_node<CLuaNodes::NumberNode>();
                auto& node = parser_context.get_node_from_handle<CLuaNodes::NumberNode>(node_handle);
                node.value = static_cast<long double>(lexer_context.current_number_integer) + lexer_context.current_number_fraction;
                parser_context.get_next_token();
                return node_handle;
            }
            case TokenType::Identifier: {
                return process_identifier_or_path(parser_context);
            }
            case TokenType::String:
            case TokenType::Char:
                return emit_syntax_error(parser_context);
            case TokenType::EndOfFile:
                return emit_syntax_error(parser_context);
            default:
                return emit_syntax_error(parser_context);
            }
        };
    };  

    void Parser::print_node_tree(NodeHandle node_handle,size_t current_depth){

        if (parser_context.is_error_node(node_handle))
        {
            print_error_node(node_handle,current_depth);
        } else {
            print_valid_node(node_handle,current_depth);
        };
    };

    NodeHandle Parser::generate_AST(){
     auto ast_root = Expression::process_atom(parser_context);

     if (!parser_context.is_error_node(ast_root) &&
         parser_context.see_current_token().token_type != TokenType::EndOfFile)
     {
         return Expression::emit_syntax_error(parser_context);
     }

     return ast_root;
    }
}