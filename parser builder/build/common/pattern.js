"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.MatchCharToken = exports.MatchStringToken = exports.MatchSymbolToken = exports.MatchNumericToken = exports.MatchIdentifierToken = exports.OptionalPattern = exports.ChoicePattern = exports.Pattern = exports.BasePattern = void 0;
class BasePattern {
    constructor() {
        this.class_name = this.constructor.name;
    }
}
exports.BasePattern = BasePattern;
class Pattern extends BasePattern {
    constructor() {
        super();
        this.pattern_list = new Array();
    }
    ;
    insert_pattern(pattern) {
        this.pattern_list.push(pattern);
    }
    ;
    get_yield_type() {
        return "NodeHandle";
    }
}
exports.Pattern = Pattern;
;
class ChoicePattern extends Pattern {
    constructor() {
        super();
    }
    get_yield_type() {
        if (this.pattern_list.length <= 0) {
            return "None";
        }
        const first_type = this.pattern_list[0].get_yield_type();
        for (const pattern of this.pattern_list) {
            if (pattern.get_yield_type() !== first_type) {
                throw new Error(`[MetaCompiler Type Error] ChoicePattern mismatch. ` +
                    `Alternative paths must yield the same type. Expected: ${first_type}, got: ${pattern.get_yield_type()}`);
            }
        }
        return first_type;
    }
}
exports.ChoicePattern = ChoicePattern;
class OptionalPattern extends Pattern {
    constructor() {
        super();
    }
    get_yield_type() {
        if (this.pattern_list.length === 0) {
            return "None";
        }
        if (this.pattern_list.length === 1) {
            return this.pattern_list[0].get_yield_type();
        }
        return "NodeHandle";
    }
}
exports.OptionalPattern = OptionalPattern;
class MatchIdentifierToken extends BasePattern {
    constructor() {
        super();
    }
    get_yield_type() {
        return "NodeHandle";
    }
}
exports.MatchIdentifierToken = MatchIdentifierToken;
class MatchNumericToken extends BasePattern {
    constructor() {
        super();
    }
    get_yield_type() {
        return "NumericToken";
    }
}
exports.MatchNumericToken = MatchNumericToken;
class MatchSymbolToken extends BasePattern {
    constructor(expected_symbol) {
        super();
        this.expected_symbol = expected_symbol;
    }
    get_yield_type() {
        return "SymbolToken";
    }
}
exports.MatchSymbolToken = MatchSymbolToken;
class MatchStringToken extends BasePattern {
    constructor() {
        super();
    }
    get_yield_type() {
        return "StringToken";
    }
}
exports.MatchStringToken = MatchStringToken;
class MatchCharToken extends BasePattern {
    constructor() {
        super();
    }
    get_yield_type() {
        return "CharToken";
    }
}
exports.MatchCharToken = MatchCharToken;
