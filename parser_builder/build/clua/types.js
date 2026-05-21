"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.symbol_to_raw = symbol_to_raw;
exports.is_symbol = is_symbol;
exports.keyword_to_raw = keyword_to_raw;
exports.is_keyword = is_keyword;
const CLuaSymbolNamespace = "SymbolKind";
const symbol_to_raw_map = {
    "+=": "PlusEqual",
    "++": "DoublePlus",
    "--": "DoubleMinus",
    "-=": "MinusEqual",
    "*=": "StarEqual",
    "/=": "SlashEqual",
    "%=": "PercentEqual",
    "==": "EqualEqual",
    "!=": "NotEqual",
    "<=": "LessEqual",
    ">=": "GreaterEqual",
    "&&": "LogicalAnd",
    "||": "LogicalOr",
    "->": "Arrow",
    "&": "BitAnd",
    "|": "BitOr",
    "^": "BitXor",
    "~": "BitNot",
    "<<": "BitLShift",
    ">>": "BitRShift",
    "&=": "BitAndEqual",
    "|=": "BitOrEqual",
    "^=": "BitXorEqual",
    "<<=": "BitLShiftEqual",
    ">>=": "BitRShiftEqual",
    "+": "Plus",
    "-": "Minus",
    "*": "Star",
    "/": "Slash",
    "%": "Percent",
    "=": "Equal",
    "<": "Less",
    ">": "Greater",
    "!": "Bang",
    ",": "Comma",
    ".": "Dot",
    ";": "Semicolon",
    ":": "Colon",
    "(": "LParen",
    ")": "RParen",
    "{": "LBrace",
    "}": "RBrace",
    "[": "LBracket",
    "]": "RBracket",
    "?": "Question",
    "?=": "TernaryAssign",
    "@": "AtSign",
    "::": "DoubleColon",
};
function symbol_to_raw(symbol) {
    return `${CLuaSymbolNamespace}::${symbol_to_raw_map[symbol]}`;
}
;
function is_symbol(symbol) {
    return symbol in symbol_to_raw_map;
}
;
const CLuaKeywordNamespace = "KeywordKind";
const keyword_to_raw_map = {
    "if": "If",
    "else": "Else",
    "for": "For",
    "while": "While",
    "do": "Do",
    "switch": "Switch",
    "case": "Case",
    "default": "Default",
    "break": "Break",
    "continue": "Continue",
    "return": "Return",
    "const": "Const",
    "static": "Static",
    "template": "Template",
    "class": "Class",
    "struct": "Struct",
    "enum": "Enum",
    "union": "Union",
    "public": "Public",
    "private": "Private",
    "protected": "Protected",
    "virtual": "Virtual",
    "inline": "Inline",
    "using": "Using",
    "namespace": "Namespace",
    "volatile": "Volatile",
    "mutable": "Mutable",
    "extern": "Extern",
    "friend": "Friend",
    "new": "New",
    "delete": "Delete",
    "true": "True",
    "false": "False",
    "nullptr": "Nil",
    "typedef": "Typedef",
    "auto": "Auto",
    "decltype": "Decltype",
    "comptime": "Comptime",
    "static_assert": "StaticAssert",
    "sizeof": "Sizeof",
    "lua_embed": "LuaEmbed",
    "import": "Import",
    "export": "Export",
    "static_cast": "StaticCast",
    "reinterpret_cast": "ReinterpretCast",
};
function keyword_to_raw(keyword) {
    return `${CLuaKeywordNamespace}::${keyword_to_raw_map[keyword]}`; //tbh I am impressed that typescript has this advanced reasoning
}
;
function is_keyword(keyword) {
    return keyword in keyword_to_raw_map;
}
;
