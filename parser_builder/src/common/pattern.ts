export type PatternYieldType = "NodeHandle" | "CharToken" | "StringToken" | "NumericToken" | "SymbolToken" | "Infer" | "None" 

export type PatternType = BasePattern | Pattern;

export abstract class BasePattern {
    public readonly class_name: string;

    constructor() {
        this.class_name = this.constructor.name;
    }

    abstract get_yield_type(): PatternYieldType;
}

export class Pattern extends BasePattern {
    pattern_list: Array<BasePattern> = new Array<BasePattern>();
    
    constructor()
    {
        super();
    };

    insert_pattern(pattern: PatternType)
    {
        this.pattern_list.push(pattern);
    };

    get_yield_type(): PatternYieldType {
        return "NodeHandle";
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

export class OptionalPattern extends Pattern {
    constructor() {
        super();
    }

    public override get_yield_type(): PatternYieldType {
        return "Infer"; 
    }
}

export class MatchIdentifierToken extends BasePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "NodeHandle"; 
    }
}

export class MatchNumericToken extends BasePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "NumericToken"; 
    }
}

export class MatchSymbolToken extends BasePattern {
    constructor(public readonly expected_symbol?: string) {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "SymbolToken" 
    }
}

export class MatchStringToken extends BasePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "StringToken"; 
    }
}

export class MatchCharToken extends BasePattern {
    constructor() {
        super();
    }

    get_yield_type(): PatternYieldType { 
        return "CharToken"; 
    }
}