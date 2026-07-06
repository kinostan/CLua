export enum PatternYieldType {
    NodeHandle, 
    TokenSpan,
    Symbol,
    None
};

export type PatternType = BasePattern;

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
    error_id: number = -1;

    get_children(): Array<PatternType> {
        return [];
    }

    with_error(error_id: number): this
    {
        this.error_id = error_id;
        return this;
    };
};
