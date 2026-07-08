import { BasePattern, PatternType, PatternYieldType, PrimitivePattern } from "./base";

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
    public inclusive: boolean = true; //does include terminating pattern
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

    public set_inclusive(inclusive: boolean): this
    {
        this.inclusive = inclusive;
        return this;
    };

    // Children are both your terminators and your whitelisted elements
    get_children(): Array<PatternType> {
        return [...this.terminators, ...this.interrput_patterns];
    }

    yields_node(node_id: number): this {
        this.node_id = node_id;
        return this;
    };
}

