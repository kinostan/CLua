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

    NodeHandle Parser::generate_AST(){
        return get_root_ast_node(parser_context);
    }
}