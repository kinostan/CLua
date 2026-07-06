import { PrimitivePattern, PatternYieldType } from "./base";

class CharRangeVal{
    char_a: number;
    char_b: number;

    constructor(char_a: number,char_b: number)
    {
        this.char_a = char_a;
        this.char_b = char_b;
    };
};

export class CharRange extends PrimitivePattern {
    char_ranges: Array<CharRangeVal>;
    
    constructor()
    {
        super();
        this.char_ranges = new Array<CharRangeVal>();
        this.yield_type = PatternYieldType.TokenSpan;
    };

    private insert_char_range_base(new_range: CharRangeVal)
    {
        let merged_char_ranges = new Array<CharRangeVal>();
        for (let range_index = 0 ;range_index < this.char_ranges.length;range_index++) {
            const range: CharRangeVal = this.char_ranges[range_index]!;

            const is_smaller = new_range.char_b < range.char_a;
            const is_overlapping = Math.max(new_range.char_a, range.char_a) <= Math.min(new_range.char_b, range.char_b);

            if (is_overlapping)
            {
                throw new Error("No overlap allowed");
            };

            if (!is_overlapping && is_smaller)
            {
                merged_char_ranges.push(new_range);
                new_range = range;
            } else {
                merged_char_ranges.push(range);
            };
        }
        merged_char_ranges.push(new_range);
        this.char_ranges = merged_char_ranges;
    };

    insert_range_def(char_range: CharRange): this
    {
        for (let range_index = 0 ;range_index < char_range.char_ranges.length;range_index++) {
            this.insert_char_range_base(char_range.char_ranges[range_index]!);
        };
        return this;
    };

    insert_range(a: string, b: string): this
    {
        if (a.length !== 1 || b.length !== 1)
        {
            throw new Error("Expected something else");
        };

        let char_a = a.charCodeAt(0);
        let char_b = b.charCodeAt(0);

        if (char_a > char_b)
        {
            [char_a, char_b] = [char_b, char_a];
        };

        this.insert_char_range_base(new CharRangeVal(char_a,char_b));
        
        return this;
    };
};

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
