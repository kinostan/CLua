"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.build_parser = build_parser;
const emitter_1 = require("#root/common/emitter");
const descriptions_1 = require("#root/common/descriptions");
var CLua;
(function (CLua) {
    function parser_headers(emitter) {
        emitter.emit(`#include "parser.hpp"`);
        emitter.emit(``);
        emitter.emit(`#include <common/clua/tokens.hpp>`);
        emitter.emit(`#include <common/base.hpp>`);
        emitter.emit(``);
    }
    ;
    function emit_clua_base_start(emitter) {
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
    }
    ;
    function emit_clua_base_end(emitter) {
        emitter.emit(`NodeHandle Parser::generate_AST(){`);
        emitter.step_indent();
        emitter.emit(`return get_root_ast_node(parser_context);`);
        emitter.step_dedent();
        emitter.emit(`};`);
    }
    ;
    function emit_parser_cpp_logic(emitter) {
    }
    ;
    function emit_parser_cpp() {
        const emitter = new emitter_1.ParserEmitter();
        const namespace = new descriptions_1.NamespaceDescription("CLuaASTParser");
        parser_headers(emitter);
        emitter.emit_namespace(namespace, () => {
            emit_clua_base_start(emitter);
            emit_parser_cpp_logic(emitter);
            emit_clua_base_end(emitter);
        });
        console.log(emitter.code);
    }
    CLua.emit_parser_cpp = emit_parser_cpp;
    ;
    function emit_ast_nodes() {
    }
    CLua.emit_ast_nodes = emit_ast_nodes;
    ;
    function emit_keywords() {
    }
    CLua.emit_keywords = emit_keywords;
    ;
    function emit_symbols() {
    }
    CLua.emit_symbols = emit_symbols;
    ;
})(CLua || (CLua = {}));
;
;
function build_parser() {
    CLua.emit_symbols();
    CLua.emit_keywords();
    CLua.emit_ast_nodes();
    CLua.emit_parser_cpp();
}
;
