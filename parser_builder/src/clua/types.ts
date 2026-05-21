
const CLuaSymbolNamespace = "SymbolKind";

export type ValidSymbol = "+=" | "++" | "--" | "-=" | "*=" | "/=" | "%=" | "==" | "!=" | "<=" | ">=" | "&&" | "||" | "->" | "&" | "|" | "^" | "~" | "<<" | ">>" | "&=" | "|=" | "^=" | "<<=" | ">>=" | "+" | "-" | "*" | "/" | "%" | "=" | "<" | ">" | "!" | "," | "." | ";" | ":" | "(" | ")" | "{" | "}" | "[" | "]" | "?" | "?=" | "@" | "::"

type RawSymbolKind =
    | "DoublePlus"       | "PlusEqual"       | "DoubleMinus"      | "MinusEqual"
    | "StarEqual"        | "SlashEqual"      | "PercentEqual"     | "EqualEqual"
    | "NotEqual"         | "LessEqual"       | "GreaterEqual"     | "LogicalAnd"
    | "LogicalOr"        | "Arrow"           | "BitAnd"           | "BitOr"
    | "BitXor"           | "BitNot"          | "BitLShift"        | "BitRShift"
    | "BitAndEqual"      | "BitOrEqual"      | "BitXorEqual"      | "BitLShiftEqual"
    | "BitRShiftEqual"   | "Plus"            | "Minus"            | "Star"
    | "Slash"            | "Percent"         | "Equal"            | "Less"
    | "Greater"          | "Bang"            | "Dot"              | "Comma"
    | "Semicolon"        | "Colon"           | "LParen"           | "RParen"
    | "LBrace"           | "RBrace"          | "LBracket"         | "RBracket"
    | "Question"         | "TernaryAssign"   | "AtSign"           | "DoubleColon";

export type SymbolKindType = `${typeof CLuaSymbolNamespace}::${RawSymbolKind}`;

const symbol_to_raw_map: Record<ValidSymbol, RawSymbolKind> = {
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
    ">>=" : "BitRShiftEqual",
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

export function symbol_to_raw(symbol: ValidSymbol): SymbolKindType
{
    return `${CLuaSymbolNamespace}::${symbol_to_raw_map[symbol]}`;
};

export function is_symbol(symbol: string): boolean {
    return symbol in symbol_to_raw_map;
};

const CLuaKeywordNamespace = "KeywordKind";

export type ValidKeyword =
    | "if" | "else" | "for" | "while" | "do" | "switch" | "case" | "default"
    | "break" | "continue" | "return" | "const" | "static" | "template" | "class" | "struct"
    | "enum" | "union" | "public" | "private" | "protected" | "virtual" | "inline" | "using"
    | "namespace" | "volatile" | "mutable" | "extern" | "friend" | "new" | "delete"
    | "true" | "false" | "nullptr" | "typedef" | "auto" | "decltype" | "comptime"
    | "static_assert" | "sizeof" | "lua_embed" | "import" | "export"
    | "static_cast" | "reinterpret_cast";

type RawKeyword =
    | "If" | "Else" | "For" | "While" | "Do" | "Switch" | "Case" | "Default"
    | "Break" | "Continue" | "Return" | "Const" | "Static" | "Template" | "Class" | "Struct"
    | "Enum" | "Union" | "Public" | "Private" | "Protected" | "Virtual" | "Inline" | "Using"
    | "Namespace" | "Volatile" | "Mutable" | "Extern" | "Friend" | "New" | "Delete"
    | "True" | "False" | "Nil" | "Typedef" | "Auto" | "Decltype" | "Comptime"
    | "StaticAssert" | "Sizeof" | "LuaEmbed" | "Import" | "Export" | "StaticCast" | "ReinterpretCast";

export type KeywordKindType = `${typeof CLuaKeywordNamespace}::${RawKeyword}`;

const keyword_to_raw_map: Record<ValidKeyword, RawKeyword> = {
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

export function keyword_to_raw(keyword: ValidKeyword): KeywordKindType
{
    return `${CLuaKeywordNamespace}::${keyword_to_raw_map[keyword]}`; //tbh I am impressed that typescript has this advanced reasoning
};

export function is_keyword(keyword: string): boolean {
    return keyword in keyword_to_raw_map;
};