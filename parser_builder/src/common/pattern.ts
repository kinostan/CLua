import { ErrorType } from "./parser_error";

export type PatternYieldType = "NodeHandle" | "CharToken" | "StringToken" | "NumericToken" | "SymbolToken" | "None" 

export type PatternType = BasePattern | Pattern;

export abstract class BasePattern {
    class_name: string;

    constructor() {
        this.class_name = this.constructor.name;
    }

    abstract get_yield_type(): PatternYieldType;
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

    get_yield_type(): PatternYieldType {
        return "NodeHandle";
    }

    get_children(): Array<PatternType> {
        return this.pattern_list.slice();
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

    get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

export class MatchKeywordToken extends PrimitivePattern {
    constructor(expected_keyword: string) {
        super();
        /* 
            I really should create a map to keep track of these pattern_names and keep 
            the track of reference counts or something to ensure uniqueness of these keys
            But the problem would be that I need to register then these patterns which is 
            another set of expressions and complexity which I want to avoid (maybe it's a good choice).
        
            Therefore I am going for simplicitly here and hope nobody ever using it
            including me is going to set_pattern_name to a pattern as "keyword_[pattern_that_has_keyword]" 
            because that would be stupid. Same goes for all other primitives.
        */
        this.set_pattern_name(`keyword_${expected_keyword}`);
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

export enum NumericTokenType {
    float,
    integer,
};

export class MatchNumericToken extends PrimitivePattern {
    number_type: NumericTokenType = NumericTokenType.integer;

    constructor(number_type: NumericTokenType) {
        super();

        this.number_type = number_type;
    }

    get_yield_type(): PatternYieldType { 
        return "NumericToken"; 
    }
}

export class MatchSymbolToken extends PrimitivePattern {
    expected_symbol: string = "";

    constructor(expected_symbol: string) {
        super();
        this.expected_symbol = expected_symbol;

        /* 
        It's a bit tricky one maybe I should turn symbol stream into
        "wordified" symbol stream to maintain the style that currently is in
        common/clua/symbol_classifier.hpp 

        "{"++", SymbolKind::DoublePlus}, {"+=", SymbolKind::PlusEqual},
        {"--", SymbolKind::DoubleMinus}, {"-=", SymbolKind::MinusEqual},
        {"*=", SymbolKind::StarEqual}, {"/=", SymbolKind::SlashEqual},
        {"%=", SymbolKind::PercentEqual}"

        */

        this.set_pattern_name(`symbol_${expected_symbol}`);
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

export namespace PatternOperators {
    export function is_equal(left_pattern: PatternType, right_pattern: PatternType): boolean {
        if (left_pattern === right_pattern) return true;

        if (!left_pattern || !right_pattern) return false;
        if (left_pattern.constructor !== right_pattern.constructor) return false;

        if (left_pattern instanceof MatchSymbolToken && right_pattern instanceof MatchSymbolToken) {
            return left_pattern.expected_symbol === right_pattern.expected_symbol;
        }

        if (left_pattern instanceof PatternSwitchParser && right_pattern instanceof PatternSwitchParser) {
            return left_pattern.target_profile_id === right_pattern.target_profile_id;
        }

        // Handle Quantity Containers
        if (left_pattern instanceof QuantityPattern && right_pattern instanceof QuantityPattern) {
            if (left_pattern.min !== right_pattern.min || left_pattern.max !== right_pattern.max) return false;
            return is_equal(left_pattern.child_pattern as PatternType, right_pattern.child_pattern as PatternType);
        }

        // Handle Sequence and Alternative Paths (Pattern & ChoicePattern)
        if (left_pattern instanceof Pattern && right_pattern instanceof Pattern) {
            
            // A pattern yielding an AST Node is never identical to left_pattern raw structural path
            if (left_pattern.node_id !== right_pattern.node_id) return false;

            const a_children = left_pattern.pattern_list;
            const b_children = right_pattern.pattern_list;

            if (a_children.length !== b_children.length) return false;

            for (let i = 0; i < a_children.length; i++) {
                if (!is_equal(a_children[i]!, b_children[i]!)) {
                    return false;
                }
            }
            return true;
        }

        return left_pattern.class_name == right_pattern.class_name;
    }

    /*
    This set of commands is strictly for collapse_pattern operation 
    */

    class Command {
        is_a(class_constructor: any)
        {
            return this.constructor === class_constructor;
        };
    };

    class CommandDelete extends Command {
        pattern: PatternType;
        constructor(delete_pattern: Pattern)
        {
            super();
            this.pattern = delete_pattern;
        };
    };

    /*
        CommandMerge -- Merge(pattern_a,pattern_b) and pattern_a is merged into pattern_b if 
        possible. The operation is valid only when pattern_b is a ChoicePattern

        CommandInsert 
    */

    type ASTCommand = CommandDelete;

    function execute_command_set(pattern: PatternType, command_set: Array<ASTCommand>)
    {

    };

    export function collapse_pattern(pattern: PatternType): PatternType
    {
        /* 
            A collapsable patterns are strictly those patterns which logic withing the bounds
            of pattern.ts expressivness allow to compress that logic by reducing the amount of grammar 
            expression nodes.
        */
        return pattern;
    };
};