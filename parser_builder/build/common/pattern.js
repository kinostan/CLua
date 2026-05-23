"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.QuantityPattern = exports.OrPattern = exports.Pattern = exports.CharPattern = exports.NumberPattern = exports.StringPattern = exports.PrimitivePattern = exports.KeywordListPattern = exports.KeywordPattern = exports.SymbolMapPattern = exports.BasePattern = void 0;
const types_1 = require("#clua/types");
class BasePattern {
    constructor(error_emitter_message_id) {
        this.node_template_id = -1;
        this.error_emitter_message_id = error_emitter_message_id;
    }
    ;
    set_node_template_id(id) {
        this.node_template_id = id;
        return this;
    }
    ;
}
exports.BasePattern = BasePattern;
;
class SymbolMapPattern extends BasePattern {
    constructor(error_emitter_message_id, ...symbol_list) {
        super(error_emitter_message_id);
        this.symbol_map = new Map();
        for (const symbol of symbol_list) {
            if (!(0, types_1.is_symbol)(symbol)) {
                throw new Error(`invalid clua symbol: ${symbol}`);
            }
            ;
            this.symbol_map.set(symbol, true);
        }
        ;
    }
    ;
    has_symbol(symbol) {
        return this.symbol_map.has(symbol);
    }
    ;
}
exports.SymbolMapPattern = SymbolMapPattern;
;
class KeywordPattern extends BasePattern {
    constructor(error_emitter_message_id, keyword) {
        super(error_emitter_message_id);
        this.keyword = keyword;
    }
    ;
}
exports.KeywordPattern = KeywordPattern;
;
class KeywordListPattern extends BasePattern {
    constructor(error_emitter_message_id, name) {
        super(error_emitter_message_id);
        this.keyword_pattern_name = "";
        this.keyword_list = new Map();
        this.keyword_pattern_name = name;
    }
    ;
    insert_keyword(keyword) {
        this.keyword_list.set(keyword, true);
        return this;
    }
    ;
}
exports.KeywordListPattern = KeywordListPattern;
;
class PrimitivePattern extends BasePattern {
    constructor(error_emitter_message_id, primitive_pattern_name) {
        super(error_emitter_message_id);
        this.primitive_pattern_name = "";
        this.primitive_pattern_name = primitive_pattern_name;
    }
    ;
}
exports.PrimitivePattern = PrimitivePattern;
;
class StringPattern extends PrimitivePattern {
    constructor(error_emitter_message_id) {
        super(error_emitter_message_id, "string");
    }
    ;
}
exports.StringPattern = StringPattern;
;
class NumberPattern extends PrimitivePattern {
    constructor(error_emitter_message_id) {
        super(error_emitter_message_id, "number");
    }
    ;
}
exports.NumberPattern = NumberPattern;
;
class CharPattern extends PrimitivePattern {
    constructor(error_emitter_message_id) {
        super(error_emitter_message_id, "char");
    }
    ;
}
exports.CharPattern = CharPattern;
;
class Pattern extends BasePattern {
    constructor(error_emitter_message_id, pattern_name) {
        super(error_emitter_message_id);
        this.pattern_name = "";
        this.pattern_list = new Array();
        this.pattern_name = pattern_name;
    }
    ;
}
exports.Pattern = Pattern;
;
class OrPattern extends BasePattern {
    constructor(error_emitter_message_id, pattern_name) {
        super(error_emitter_message_id);
        this.or_pattern_name = "";
        this.accepted_patterns = new Array();
        this.or_pattern_name = pattern_name;
    }
    ;
    add_pattern(pattern) {
        this.accepted_patterns.push(pattern);
        return this;
    }
    ;
}
exports.OrPattern = OrPattern;
;
class QuantityPattern extends BasePattern {
    constructor(pattern, minimum, maximum) {
        super(0);
        this.minimum = 0;
        this.maximum = -1;
        this.pattern = pattern;
        this.minimum = minimum;
        this.maximum = maximum;
    }
    ;
}
exports.QuantityPattern = QuantityPattern;
;
console.log((0, types_1.symbol_to_raw)("!="));
