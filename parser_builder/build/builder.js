"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.build_parser = build_parser;
require("#root/common/pattern");
require("#config/pattern");
require("#config/nodes");
const emitter_1 = require("#root/common/emitter");
function emit_parser_cpp() {
    const emitter = new emitter_1.ParserEmitter();
    console.log(emitter.code);
}
;
function emit_ast_nodes() {
}
;
function build_parser() {
    //process nodes and then pattern, then emit the code
    emit_parser_cpp();
}
;
