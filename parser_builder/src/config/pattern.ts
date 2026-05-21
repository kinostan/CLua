import * as BasicPatterns from "../clua/pattern";
import * as Errors from "../config/errors";

export let clua_pattern_map: Map<string, BasicPatterns.PatternType> = new Map();

function set_pattern(name: string, pattern: BasicPatterns.PatternType): BasicPatterns.PatternType {
    clua_pattern_map.set(name, pattern);
    return pattern;
}

let NumberPattern = set_pattern("number", new BasicPatterns.NumberPattern(
    Errors.NumberPatternMessageId)
);

let StringPattern = set_pattern("string", new BasicPatterns.StringPattern(
    Errors.StringPatternMessageId)
);

let CharPattern = set_pattern("char", new BasicPatterns.CharPattern(
    Errors.CharPatternMessageId)
);
 