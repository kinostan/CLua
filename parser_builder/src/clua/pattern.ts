import { ValidSymbol, SymbolKindType  } from "./types";

type PatternType = QuantityPattern | SymbolMapPattern | KeywordListPattern | Pattern;

export const NormalizedSymbols: Map<ValidSymbol, SymbolKindType> = new Map([
    ["++", "SymbolKind::DoublePlus"],   ["+=", "SymbolKind::PlusEqual"],
    ["--", "SymbolKind::DoubleMinus"],  ["-=", "SymbolKind::MinusEqual"],
    ["*=", "SymbolKind::StarEqual"],   ["/=", "SymbolKind::SlashEqual"],
    ["%=", "SymbolKind::PercentEqual"],  ["==", "SymbolKind::EqualEqual"],
    ["!=", "SymbolKind::NotEqual"],    ["<=", "SymbolKind::LessEqual"],
    [">=", "SymbolKind::GreaterEqual"], ["&&", "SymbolKind::LogicalAnd"],
    ["||", "SymbolKind::LogicalOr"],   ["->", "SymbolKind::Arrow"],
    ["&", "SymbolKind::BitAnd"],       ["|", "SymbolKind::BitOr"],
    ["^", "SymbolKind::BitXor"],       ["~", "SymbolKind::BitNot"],
    ["<<", "SymbolKind::BitLShift"],   [">>", "SymbolKind::BitRShift"],
    ["&=", "SymbolKind::BitAndEqual"],  ["|=", "SymbolKind::BitOrEqual"],
    ["^=", "SymbolKind::BitXorEqual"],  ["<<=", "SymbolKind::BitLShiftEqual"],
    [">>=", "SymbolKind::BitRShiftEqual"], ["+", "SymbolKind::Plus"],
    ["-", "SymbolKind::Minus"],        ["*", "SymbolKind::Star"],
    ["/", "SymbolKind::Slash"],        ["%", "SymbolKind::Percent"],
    ["=", "SymbolKind::Equal"],        ["<", "SymbolKind::Less"],
    [">", "SymbolKind::Greater"],      ["!", "SymbolKind::Bang"],
    [".", "SymbolKind::Dot"],          [",", "SymbolKind::Comma"],
    [";", "SymbolKind::Semicolon"],    [":", "SymbolKind::Colon"],
    ["(", "SymbolKind::LParen"],       [")", "SymbolKind::RParen"],
    ["{", "SymbolKind::LBrace"],       ["}", "SymbolKind::RBrace"],
    ["[", "SymbolKind::LBracket"],     ["]", "SymbolKind::RBracket"],
    ["?", "SymbolKind::Question"],     ["?=", "SymbolKind::TernaryAssign"],
    ["@", "SymbolKind::AtSign"],       ["::", "SymbolKind::DoubleColon"]
]);

export class BasePattern {
    error_emitter_message_id: number;
    node_template_id: number = -1;
    constructor(error_emitter_message_id: number)
    {
        this.error_emitter_message_id = error_emitter_message_id;
    };

    set_node_template_id(id: number) {
        this.node_template_id = id;
    };
};

export class SymbolMapPattern extends BasePattern {
    symbol_map: Map<ValidSymbol,boolean> = new Map<ValidSymbol,boolean>();

    constructor(error_emitter_message_id: number, ...symbol_list: ValidSymbol[]) {
        super(error_emitter_message_id);
        for (const symbol of symbol_list) {
            
            if (!NormalizedSymbols.has(symbol))
            {
                throw new Error(`invalid clua symbol: ${symbol}`);
            };

            this.symbol_map.set(symbol,true);
       };
    };
    
    has_symbol(symbol: ValidSymbol): boolean{
        return this.symbol_map.has(symbol);
    };
};

export class KeywordPattern<T extends string = string> extends BasePattern{
    keyword: T;
    constructor(error_emitter_message_id: number, keyword: T)
    {
        super(error_emitter_message_id);
        this.keyword = keyword;
    };
};

export class KeywordListPattern<T extends string = string> extends BasePattern{
    keyword_pattern_name: string = "";
    keyword_list: Map<string,boolean> = new Map<string,boolean>();

    constructor(error_emitter_message_id: number, name: string) { 
        super(error_emitter_message_id);
        this.keyword_pattern_name = name;
    };

    insert_keyword(keyword: T)
    {
        this.keyword_list.set(keyword,true);
    };
};

export class Pattern extends BasePattern {    
    pattern_name: string = "";
    pattern_list: Array<PatternType> = new Array<PatternType>();

    constructor(error_emitter_message_id: number,pattern_name: string)
    {
        super(error_emitter_message_id);
        this.pattern_name = pattern_name;
    };
};

export class QuantityPattern 
{
    pattern: PatternType;
    minimum = 0;
    maximum = -1;
    constructor(pattern: PatternType,minimum: number, maximum: number)
    {
        this.pattern = pattern;
        this.minimum = minimum;
        this.maximum = maximum;
    };
};