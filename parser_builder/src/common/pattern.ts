import { ErrorType } from "./parser_error";

export type PatternYieldType = "NodeHandle" | "WordToken" | "NumericToken" | "SymbolToken" | "WhitespaceToken" | "NewLineToken" | "None" 

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
        return "WordToken"; 
    }
}

export class MatchIdentifierToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "WordToken"; 
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
    expected_symbol: string = "";
    symbol_label: string = "";

    constructor(expected_symbol: string, symbol_label: string = "") {
        super();
        this.expected_symbol = expected_symbol;
        this.symbol_label = symbol_label;

        /* 
        It's a bit tricky one maybe I should turn symbol stream into
        "wordified" symbol stream to maintain the style that currently is in
        common/clua/symbol_classifier.hpp 

        "{"++", SymbolKind::DoublePlus}, {"+=", SymbolKind::PlusEqual},
        {"--", SymbolKind::DoubleMinus}, {"-=", SymbolKind::MinusEqual},
        {"*=", SymbolKind::StarEqual}, {"/=", SymbolKind::SlashEqual},
        {"%=", SymbolKind::PercentEqual}"
        */
        const name_suffix = symbol_label ? `_${symbol_label}` : "";
        this.set_pattern_name(`symbol_${expected_symbol}${name_suffix}`);
    }

    get_yield_type(): PatternYieldType { 
        return "SymbolToken" 
    }
}

export class MatchWhitespaceToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType {
        return "WhitespaceToken";
    }
}

export class MatchNewLineToken extends PrimitivePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType {
        return "NewLineToken";
    }
}

/**
 * Extracts a symbol value from a pattern match.
 * This acts as a 'binding marker' for the yields_node() logic. It signals that
 * the parser should extract the symbol information from the immediate child
 * (depth=1) and map it to a non-NodeHandle field in the C++ AST node.
*/
export class ExtractSymbol extends BasePattern {
    constructor(
        public readonly child_pattern: BasePattern
    ) {
        super();
        this.set_pattern_name(`extract_symbol`);
    }

    public override get_yield_type(): PatternYieldType {
        return "SymbolToken";
    }

    public override get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

/**
 * Captures the raw TokenSpan of the child pattern and wraps it in a NodeHandle.
 * Essential for Numeric Literals and other constructs where you want validation
 * but only need the raw text in the AST.
*/
export class TokenSpanPattern extends BasePattern {
    constructor(
        public readonly node_id: number, 
        public readonly child_pattern: BasePattern
    ) {
        super();
        this.set_pattern_name(`token_span_node_${node_id}`);
    }

    public override get_yield_type(): PatternYieldType {
        return "NodeHandle";
    }

    public override get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

/**
 * Consumes all tokens until any of the exclude_patterns are matched.
 * Important: Per architectural rule, it MUST consume the matching exclude_pattern upon success.
*/
export class InvertedPattern extends PrimitivePattern {
    public exclude_patterns: Array<BasePattern> = new Array<BasePattern>();
    public node_id: number = -1;

    constructor(node_id: number) {
        super();
        this.node_id = node_id;
        this.set_pattern_name(`inverted_node_${node_id}`);
    }

    public insert_exclude_pattern(pattern: BasePattern): this {
        this.exclude_patterns.push(pattern);
        return this;
    }

    public override get_yield_type(): PatternYieldType { 
        return "NodeHandle"; 
    }

    get_children(): Array<PatternType> {
        return this.exclude_patterns;
    }
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

    public override get_yield_type(): PatternYieldType {
        return "None"; 
    }

    public override get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

/**
    * State Variables: Match a pattern repetition count previously captured.
*/
export class MatchContextLengthPattern extends PrimitivePattern {
    constructor(
        public readonly child_pattern: BasePattern, 
        public readonly context_key: LengthContext
    ) {
        super();
    }

    public override get_yield_type(): PatternYieldType {
        return "None"; 
    }

    public override get_children(): Array<PatternType> {
        return [this.child_pattern];
    }
}

export class LengthContext {
    constructor(){};
};