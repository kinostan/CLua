"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.build_parser = build_parser;
require("./clua/descriptions");
require("./clua/pattern");
require("./config/pattern");
require("./config/nodes");
require("./emitter");
const emitter_1 = require("./emitter");
const descriptions_1 = require("./clua/descriptions");
function emit_parser_cpp() {
    let class_desc = new descriptions_1.ClassDescription("eh", null, [
        ["field1", "Common::uint8"],
        ["field2", "CLua::NodeHandle"],
        ["field3", "CLua::NodeHandle"]
    ]);
    let emitter = new emitter_1.BaseEmitter();
    emitter.emit_class(class_desc);
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
