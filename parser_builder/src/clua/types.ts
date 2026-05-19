export type FieldSize = 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128;

type CLuaSymbolNamespace = "SymbolKind";

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

export type SymbolKindType = `${CLuaSymbolNamespace}::${RawSymbolKind}`;

type CommonTypeNamespace = "Common";
type RawNumberType = "uint8" | "uint16" 
| "uint32" | "uint64" 
| "f32" | "f64";
export type NumberType = `${CommonTypeNamespace}::${RawNumberType}`; 

type CLuaNamespace = "CLua";

type RawCLuaTypes = "NodeHandle" | "TokenGeneric" | "TokenSpan";
export type CLuaTypes = `${CLuaNamespace}::${RawCLuaTypes}`;

export type Type = NumberType | CLuaTypes | string;

export const FieldTypeToSizeMap: Map<Type,FieldSize> = new Map<Type,FieldSize>();

FieldTypeToSizeMap.set("CLua::TokenSpan",32);
FieldTypeToSizeMap.set("CLua::TokenGeneric",16);
FieldTypeToSizeMap.set("CLua::NodeHandle",8);

FieldTypeToSizeMap.set("Common::f64",8);
FieldTypeToSizeMap.set("Common::f32",4);

FieldTypeToSizeMap.set("Common::uint64",8);
FieldTypeToSizeMap.set("Common::uint32",4);
FieldTypeToSizeMap.set("Common::uint16",2);
FieldTypeToSizeMap.set("Common::uint8",1);

export function set_field_type(field_type: Type, size: FieldSize) {
    if (FieldTypeToSizeMap.has(field_type))
    {
        throw new Error(`trying to set the size of the same field twice ${field_type}`);
    };
    FieldTypeToSizeMap.set(field_type,size);
};
