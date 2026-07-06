import { ErrorType } from "./parser_error";

export enum PatternYieldType {
    NodeHandle, 
    TokenSpan,
    Symbol,
    None
};

export type PatternType = BasePattern | Pattern;

export abstract class BasePattern {
    class_name: string;
    yield_type: PatternYieldType = PatternYieldType.None;

    constructor() {
        this.class_name = this.constructor.name;
    }

    get_yield_type(): PatternYieldType {
        return this.yield_type;
    };
    abstract get_children(): Array<PatternType>;

    set_pattern_name(pattern_name: string): this
    {
        this.class_name = pattern_name;
        return this;
    };

    get_pattern_name(): string
    {
        return this.class_name;
    };
}

export abstract class PrimitivePattern extends BasePattern {
    error: ErrorType = ErrorType.None;
    get_children(): Array<PatternType> {
        return [];
    }

    with_error(error: ErrorType): this
    {
        this.error = error;
        return this;
    };
};

export class Pattern extends PrimitivePattern {
    pattern_list: Array<PatternType> = new Array<PatternType>();
    node_id: number = -1;

    constructor()
    {
        super();
    };

    insert_pattern(pattern: PatternType): this
    {
        this.pattern_list.push(pattern);
        return this;
    };

    yields_node(node_id: number): this
    {
        this.node_id = node_id;
        return this;
    };

    get_children(): Array<PatternType> {
        return this.pattern_list.slice();
    }
};

//Sets a clear boundary between languages and implies 
//that new keyword table is generated for the new language
export class PatternSwitchParser extends PrimitivePattern {
    constructor(public readonly target_profile_id: number) {
        super();
    }
};

//Matches in IR which pattern is being expressed and 
//handles multiple paths
export class ChoicePattern extends Pattern {
    constructor() {
        super();
    }

    public override get_yield_type(): PatternYieldType {
        if (this.pattern_list.length <= 0) {
            return PatternYieldType.None;
        }

        const first_type = this.pattern_list[0]!.get_yield_type();

        for (const pattern of this.pattern_list) {
            if (pattern.get_yield_type() !== first_type) {
                throw new Error(
                    `[MetaCompiler Type Error] ChoicePattern mismatch. ` +
                    `Alternative paths must yield the same type. Expected: ${first_type}, got: ${pattern.get_yield_type()}`
                );
            }
        }

        return first_type;
    }
}

//Implicitly creates a new field for Node of a chain of nodes of the same type
//Node should first insert a field with value using the expression
//then in the next field empty QuantityPattern is inserted
export class QuantityPattern extends PrimitivePattern {
    constructor(
        // The single pattern being quantified
        public readonly child_pattern: BasePattern,
        public readonly min: number = 0,
        // Using -1 as a sentinel value for Infinity (e.g., zero-or-more)
        public readonly max: number = -1 
    ) {
        super();

        // Validation check tailored for the -1 sentinel
        const is_max_infinite = this.max === -1;
        if (this.min < 0 || (!is_max_infinite && this.max < this.min)) {
            throw new Error(
                 `[MetaCompiler Range Error] Invalid Quantity bounds: min=${this.min}, max=${this.max}`
            );
        }
    }

    public override get_yield_type(): PatternYieldType {
        // If it's explicitly bounded to 0 iterations, it yields nothing
        if (this.max === 0) {
            return PatternYieldType.None;
        }

        // If it can execute multiple times (max is -1 or greater than 1),
        // it means the high-performance C++ generator will pack these results.
        // For now, returning its inner type keeps the pipeline moving.
        if (this.max === -1 || this.max > 1) {
            return this.child_pattern.get_yield_type(); 
        }

        // If max is exactly 1 (like the old OptionalPattern), it perfectly inherits
        // the structural type of its wrapped child asset.
        return this.child_pattern.get_yield_type();
    }

    get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

//Implicit TokenSpan
export class MatchCharacterSet extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return PatternYieldType.TokenSpan; 
    }
}

//Symbol type
/* 
    Symbol is nothing else than keywords and operators being combined together into 1 group 
*/

export class MatchSymbolPattern extends PrimitivePattern {
    expected_symbol: string = "";
    symbol_label: string = "";

    constructor(expected_symbol: string, symbol_label: string = "") {
        super();
        this.expected_symbol = expected_symbol;
        this.symbol_label = symbol_label;

        const name_suffix = symbol_label ? `_${symbol_label}` : "";
        this.set_pattern_name(`symbol_${expected_symbol}${name_suffix}`);
    }

    get_yield_type(): PatternYieldType { 
        return PatternYieldType.Symbol 
    }
}

//Functionality of InvertedPattern
//Error recovery patterns 
//  -Consume terminators
//Strings
//  -Inverted pattern allows 
//  expressing strings or even comments

//InvertedPattern yield types and implications
//When yields node
//It also creates implicitly a linked luist node type and requires the InvertedPattern
//to have a unique name to itself
//InvertedPattern must hold all detected interrupt_patterns in a linked list
//When yields symbol
//Invalid
//When yields token span
//Check if interrupt_patterns emit nodes/symbols to warn user
//if data is being lost
export class InvertedPattern extends PrimitivePattern {
    public terminators: Array<BasePattern> = new Array<BasePattern>();
    public interrput_patterns: Array<BasePattern> = new Array<BasePattern>();
    public node_id: number = -1;

    constructor() {
        super();
    }

    public insert_terminator(pattern: BasePattern): this {
        this.terminators.push(pattern);
        return this;
    }

    public insert_whitelist_pattern(pattern: BasePattern): this {
        this.interrput_patterns.push(pattern);
        return this;
    }

    public override get_yield_type(): PatternYieldType { 
        return PatternYieldType.TokenSpan; 
    }

    // Children are both your terminators and your whitelisted elements
    get_children(): Array<PatternType> {
        return [...this.terminators, ...this.interrput_patterns];
    }

    yields_node(node_id: number): this {
        this.node_id = node_id;
        return this;
    };
}

/**
 * State Variables: Capture the repetition count of a pattern match.
 * Used for LuaU Long Strings/Comments [===[ ... ]===] where the child 
 * pattern would typically be a SymbolToken("=").
*/
export class CaptureLengthPattern extends PrimitivePattern {
    constructor(
        public readonly child_pattern: BasePattern, 
        public readonly context_key: LengthContext
    ) {
        super();
    }

    public override get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

/*
* State Variables: Match a pattern repetition count previously captured.
*/
export class MatchContextLengthPattern extends PrimitivePattern {
    constructor(
        public readonly child_pattern: BasePattern, 
        public readonly context_key: LengthContext
    ) {
        super();
    }

    public override get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

export class LengthContext {
    constructor(){};
};