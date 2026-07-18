import { BasePattern, PatternType, PrimitivePattern } from "./base";

//Sets a clear boundary between languages and implies 
//that new keyword table is generated for the new language
export class PatternSwitchParser extends PrimitivePattern {
    constructor(public readonly target_profile_id: number) {
        super();
    }
};

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

