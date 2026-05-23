"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.clua_pattern_map = void 0;
const BasicPatterns = require("../common/pattern");
const Errors = require("../config/errors");
exports.clua_pattern_map = new Map();
function set_pattern(name, pattern) {
    exports.clua_pattern_map.set(name, pattern);
    return pattern;
}
let NumberPattern = set_pattern("number", new BasicPatterns.NumberPattern(Errors.NumberPatternMessageId)).set_node_template_id(0);
let StringPattern = set_pattern("string", new BasicPatterns.StringPattern(Errors.StringPatternMessageId));
let CharPattern = set_pattern("char", new BasicPatterns.CharPattern(Errors.CharPatternMessageId).set_node_template_id(0));
