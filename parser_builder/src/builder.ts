import * as Patterns from "#root/common/pattern";

import * as ConfigPatterns from "#config/pattern";
import * as ConfigNodes from "#config/nodes";

import { ParserEmitter } from "#root/common/emitter";
import { ClassDescription, NamespaceDescription } from "#root/common/descriptions";

namespace CLua {
    function parser_headers(emitter: ParserEmitter)
    {
        emitter.emit(`#include "parser.hpp"`);
        emitter.emit(``);
        emitter.emit(`#include <common/clua/tokens.hpp>`);
        emitter.emit(`#include <common/base.hpp>`);
        emitter.emit(``);
    };

    function emit_clua_base_start(emitter: ParserEmitter)
    {
        emitter.emit(`using namespace CLuaNodes;`);
        emitter.emit(``);
        emitter.emit(`using SymbolKind = CLua::SymbolKind;`);
        emitter.emit(``);
        emitter.emit(`using TokenType = CLua::TokenType;`);
        emitter.emit(`using NumberBase = CLua::NumberBase;`);
        emitter.emit(`using NumberType = CLua::NumberType;`);
        emitter.emit(`using Source = CLua::Source;`);
        emitter.emit(``);
        emitter.emit(`NodeHandle get_root_ast_node(ParserContext& parser_context);`);
        emitter.emit(``);
    };

    function emit_clua_base_end(emitter: ParserEmitter)
    {
        emitter.emit(`NodeHandle Parser::generate_AST(){`);
        emitter.step_indent();
        emitter.emit(`return get_root_ast_node(parser_context);`);
        emitter.step_dedent();
        emitter.emit(`};`);
    };

    function emit_parser_cpp_logic(emitter: ParserEmitter)
    {
        
    };

    export function emit_parser_cpp()
    {
        const emitter = new ParserEmitter();
        const namespace: NamespaceDescription = new NamespaceDescription("CLuaASTParser");

        parser_headers(emitter);
        emitter.emit_namespace(
            namespace,
            () => {
                emit_clua_base_start(emitter);
                emit_parser_cpp_logic(emitter);
                emit_clua_base_end(emitter);
            }
        );
    };

    export function emit_ast_nodes()
    {
        
    };

    export function emit_keywords()
    {

    };

    export function emit_symbols()
    {

    };
};

namespace LuaU {

};

export function build_parser()
{
    CLua.emit_symbols();
    CLua.emit_keywords();
    CLua.emit_ast_nodes();
    CLua.emit_parser_cpp();
};
