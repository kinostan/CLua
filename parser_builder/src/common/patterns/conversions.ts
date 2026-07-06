import { BasePattern, PatternYieldType, PatternType } from "./base";

/*
Objects inheriting conversion ensure that pattern actually can be converted to 
object of selected conversion type using logic although implicitly typed fields are 
allowed too.
*/
export class Conversion extends BasePattern
{
    pattern: BasePattern

    constructor(pattern: BasePattern)
    {
        super();
        this.pattern = pattern;
    };

    get_children(): Array<PatternType> {
        return [this.pattern];
    }
}

export class SpanConversion extends Conversion{    
    constructor(pattern: BasePattern)
    {
        super(pattern);
        this.yield_type = PatternYieldType.TokenSpan;
        /* 
            Any pattern can be converted to a span
        */
    };
}
export class NodeConversion extends Conversion{    
    constructor(pattern: BasePattern)
    {
        super(pattern);
        this.yield_type = PatternYieldType.NodeHandle;
        /* 
            Ensures that pattern as a whole emit validly Node
        */
    };
}
export class SymbolConversion extends Conversion{    
    constructor(pattern: BasePattern)
    {
        super(pattern);
        this.yield_type = PatternYieldType.Symbol;

        /* 
            Works only on symbol emitting patterns, and only on MatchSymbolPattern
            which is hidden within a quantity pattern or any other pattern under certain 
            conditions...
        */
    };
}

export function as_node(pattern: BasePattern): NodeConversion
{
    return new NodeConversion(pattern);
}

export function as_span(pattern: BasePattern): SpanConversion
{
    return new SpanConversion(pattern);  
};

export function as_symbol(pattern: BasePattern): SymbolConversion
{
    return new SymbolConversion(pattern);
};