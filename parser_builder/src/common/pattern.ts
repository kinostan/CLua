import { ValidSymbol, ValidKeyword, keyword_to_raw, symbol_to_raw, is_symbol } from "#clua/types";

export type PatternType = QuantityPattern | SymbolMapPattern | KeywordListPattern | Pattern | OrPattern | StringPattern | NumberPattern | CharPattern;

export class BasePattern {
    error_emitter_message_id: number;
    node_template_id: number = -1;
    constructor(error_emitter_message_id: number)
    {
        this.error_emitter_message_id = error_emitter_message_id;
    };

    set_node_template_id(id: number) {
        this.node_template_id = id;
        return this;
    };
};

export class SymbolMapPattern extends BasePattern {
    symbol_map: Map<ValidSymbol,boolean> = new Map<ValidSymbol,boolean>();

    constructor(error_emitter_message_id: number, ...symbol_list: ValidSymbol[]) {
        super(error_emitter_message_id);
        for (const symbol of symbol_list) {
            
            if (!is_symbol(symbol))
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
        return this;
    };
};

export class PrimitivePattern extends BasePattern {
    primitive_pattern_name: string = "";

    constructor(error_emitter_message_id: number, primitive_pattern_name: string)
    {
        super(error_emitter_message_id);
        this.primitive_pattern_name = primitive_pattern_name;
    };
};

export class StringPattern extends PrimitivePattern {
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id, "string");
    };
};

export class NumberPattern extends PrimitivePattern {
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id, "number");
    };
};

export class CharPattern extends PrimitivePattern {
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id, "char");
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

export class OrPattern extends BasePattern {
    or_pattern_name: string = "";
    accepted_patterns: Array<PatternType> = new Array<PatternType>();

    constructor(error_emitter_message_id: number, pattern_name: string)
    {
        super(error_emitter_message_id);
        this.or_pattern_name = pattern_name;
    };

    add_pattern(pattern: PatternType)
    {
        this.accepted_patterns.push(pattern);
        return this;
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

console.log(symbol_to_raw("!="));