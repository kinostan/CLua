import { ErrorType } from "./error";

export type PatternYieldType = "NodeHandle" | "CharToken" | "StringToken" | "NumericToken" | "SymbolToken" | "Infer" | "None" 

export type PatternType = BasePattern | Pattern;

export abstract class BasePattern {
    public readonly class_name: string;

    constructor() {
        this.class_name = this.constructor.name;
    }

    abstract get_yield_type(): PatternYieldType;
}

export abstract class PrimitivePattern extends BasePattern {
    error: ErrorType = ErrorType.NONE;
};

export class Pattern extends PrimitivePattern {
    pattern_list: Array<BasePattern> = new Array<BasePattern>();
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

    get_yield_type(): PatternYieldType {
        return "NodeHandle";
    }
};

export class PatternSwitchParser extends PrimitivePattern {
    constructor(public readonly target_profile_id: number) {
        super();
    }

    get_yield_type(): PatternYieldType {
        return "None"; 
    }
};

export class ChoicePattern extends Pattern {
    constructor() {
        super();
    }

    public override get_yield_type(): PatternYieldType {
        if (this.pattern_list.length <= 0) {
            return "None";
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
            return "None";
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
}
export class MatchKeywordToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "NodeHandle"; 
    }
}

export class MatchIdentifierToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "NodeHandle"; 
    }
}

export class MatchNumericToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "NumericToken"; 
    }
}

export class MatchSymbolToken extends PrimitivePattern {
    constructor(public readonly expected_symbol?: string) {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "SymbolToken" 
    }
}

export class MatchStringToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "StringToken"; 
    }
}

export class MatchCharToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "CharToken"; 
    }
}