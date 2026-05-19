"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.QuantityPattern = exports.Pattern = exports.KeywordListPattern = exports.KeywordPattern = exports.SymbolMapPattern = exports.BasePattern = exports.ErrorMessagePattern = exports.NormalizedSymbols = void 0;
exports.NormalizedSymbols = new Map([
    ["++", "SymbolKind::DoublePlus"], ["+=", "SymbolKind::PlusEqual"],
    ["--", "SymbolKind::DoubleMinus"], ["-=", "SymbolKind::MinusEqual"],
    ["*=", "SymbolKind::StarEqual"], ["/=", "SymbolKind::SlashEqual"],
    ["%=", "SymbolKind::PercentEqual"], ["==", "SymbolKind::EqualEqual"],
    ["!=", "SymbolKind::NotEqual"], ["<=", "SymbolKind::LessEqual"],
    [">=", "SymbolKind::GreaterEqual"], ["&&", "SymbolKind::LogicalAnd"],
    ["||", "SymbolKind::LogicalOr"], ["->", "SymbolKind::Arrow"],
    ["&", "SymbolKind::BitAnd"], ["|", "SymbolKind::BitOr"],
    ["^", "SymbolKind::BitXor"], ["~", "SymbolKind::BitNot"],
    ["<<", "SymbolKind::BitLShift"], [">>", "SymbolKind::BitRShift"],
    ["&=", "SymbolKind::BitAndEqual"], ["|=", "SymbolKind::BitOrEqual"],
    ["^=", "SymbolKind::BitXorEqual"], ["<<=", "SymbolKind::BitLShiftEqual"],
    [">>=", "SymbolKind::BitRShiftEqual"], ["+", "SymbolKind::Plus"],
    ["-", "SymbolKind::Minus"], ["*", "SymbolKind::Star"],
    ["/", "SymbolKind::Slash"], ["%", "SymbolKind::Percent"],
    ["=", "SymbolKind::Equal"], ["<", "SymbolKind::Less"],
    [">", "SymbolKind::Greater"], ["!", "SymbolKind::Bang"],
    [".", "SymbolKind::Dot"], [",", "SymbolKind::Comma"],
    [";", "SymbolKind::Semicolon"], [":", "SymbolKind::Colon"],
    ["(", "SymbolKind::LParen"], [")", "SymbolKind::RParen"],
    ["{", "SymbolKind::LBrace"], ["}", "SymbolKind::RBrace"],
    ["[", "SymbolKind::LBracket"], ["]", "SymbolKind::RBracket"],
    ["?", "SymbolKind::Question"], ["?=", "SymbolKind::TernaryAssign"],
    ["@", "SymbolKind::AtSign"], ["::", "SymbolKind::DoubleColon"]
]);
class ErrorMessagePattern {
}
exports.ErrorMessagePattern = ErrorMessagePattern;
;
class BasePattern {
    constructor(error_emitter_message_id) {
        this.error_emitter_message_id = error_emitter_message_id;
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
            if (!exports.NormalizedSymbols.has(symbol)) {
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
    }
    ;
}
exports.KeywordListPattern = KeywordListPattern;
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
class QuantityPattern {
    constructor(pattern, minimum, maximum) {
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
