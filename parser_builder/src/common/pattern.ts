import { ValidSymbol, ValidKeyword, keyword_to_raw, symbol_to_raw, is_symbol } from "#root/common/clua_types";
import {BaseEmitter} from "#common/emitter";

type ActionCallback = (base_emitter: BaseEmitter,node_reference_variable_name: string) => void;

export type PatternType = 
QuantityPattern | SymbolMapPattern | KeywordPattern | Pattern | OrPattern | 
StringPattern | NumberPattern | CharPattern | ActionPattern;

export class BasePattern {
    error_emitter_message_id: number;
    node_template_id: number = -1;
    constructor(error_emitter_message_id: number)
    {
        this.error_emitter_message_id = error_emitter_message_id;
    };

    set_node_template_id(id: number): this {
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

export class ActionPattern extends BasePattern{
    action_name: string = "";
    action_callback: ActionCallback = () => {};
    constructor(action_name: string,action_callback: ActionCallback)
    {
        super(0);
        this.action_name = action_name;
        this.action_callback = action_callback;
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

    insert_pattern(pattern: PatternType): this
    {
        this.pattern_list.push(pattern);

        return this;
    };
};

export class KeywordPattern<T extends string = string> extends Pattern{
    keyword: T;
    constructor(error_emitter_message_id: number, keyword: T)
    {
        super(error_emitter_message_id,"keyword");
        this.keyword = keyword;
    };
};

export class IdentifierPattern extends Pattern{
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id,"identifier");
        this.insert_pattern(
            new ActionPattern("create identifier token variable",(emitter: BaseEmitter,identifier_node_reference: string) => {
                emitter.emit(
                    `${identifier_node_reference}.identifier_token = see_current_token()`
                )
            })
        )    
    };
};

export class StringPattern extends Pattern {
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id, "string");
        this.insert_pattern(
            new ActionPattern("set string",(emitter: BaseEmitter,node_reference_variable_name: string) => {
                emitter.emit(
                    `${node_reference_variable_name}.string_token = parser_context.see_current_token();`
                );
            })
        )
    };
};

export class NumberPattern extends Pattern {
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id, "number");
        this.insert_pattern(
            new ActionPattern("set number",(emitter: BaseEmitter,node_reference_variable_name: string) => {
                emitter.emit(`${node_reference_variable_name}.fraction = parser_context.get_current_fraction();`);
                emitter.emit(`${node_reference_variable_name}.integer = parser_context.get_current_integer();`);
            })
        );
    };
};

export class CharPattern extends Pattern {
    constructor(error_emitter_message_id: number)
    {
        super(error_emitter_message_id, "char");
        this.insert_pattern(
            new ActionPattern("set char value",(emitter: BaseEmitter, node_reference_variable_name: string) => {
                emitter.emit(`${node_reference_variable_name}.char_value = parser_context.get_current_char_value();`);
            })
        );
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

export class QuantityPattern extends BasePattern
{
    pattern: PatternType;
    minimum = 0;
    maximum = -1;

    constructor(pattern: PatternType,minimum: number, maximum: number)
    {
        super(0);
        this.pattern = pattern;
        this.minimum = minimum;
        this.maximum = maximum;
    };
};
