import * as Patterns from "#root/common/pattern";

import * as ConfigPatterns from "#config/pattern";
import * as ConfigNodes from "#config/nodes";

import { ParserEmitter } from "#root/common/emitter";
import { ClassDescription, NamespaceDescription } from "#root/common/descriptions";

namespace CLua {
    export function emit_clua_base_start(emitter: ParserEmitter)
    {
        emitter.emit(`
            using namespace CLuaNodes;

            using SymbolKind = CLua::SymbolKind;

            using TokenType = CLua::TokenType;
            using NumberBase = CLua::NumberBase;
            using NumberType = CLua::NumberType;
            using Source = CLua::Source;

            NodeHandle get_root_ast_node(ParserContext& parser_context);
        `);
    };

    export function emit_clua_base_end(emitter: ParserEmitter)
    {
        emitter.emit(`
            NodeHandle Parser::generate_AST(){
                return get_root_ast_node(parser_context);
            }
        `);
    };

    export function emit_parser_cpp_logic(emitter: ParserEmitter)
    {
        
    };

    export function emit_parser_cpp()
    {
        const emitter = new ParserEmitter();

        const namespace: NamespaceDescription = new NamespaceDescription("CLuaASTParser");

        emitter.emit_namespace(
            namespace,
            () => {
                emit_clua_base_start(emitter);
                emit_parser_cpp_logic(emitter);
                emit_clua_base_end(emitter);
            }
        );

        console.log(emitter.code);
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
