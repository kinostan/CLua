import * as NodesConfig from "#root/config/clua/nodes_declare"
import * as PatternsConfig from "#config/clua/patterns"

import { Pattern, PatternType } from "#common/pattern";

/*
NodeArray (basically buffer with a type and the value of NodeArray is basically a virtual index 
pointing to a real index within the memory manager) 
should be added as a type to the parser.hpp when 
memory manager gets advanced enough as it fits very well for 
QuantityPattern and reduces memory costs when there's lots of nodes
*/

export class BuildContext {
    private next_var_id = 0;
    
    private variable_definitions = new Map<number, any>();
    
    private variable_dependencies = new Map<number, Set<any>>();

    public allocate_var_id(defining_block: any): number {
        const id = this.next_var_id++;
        this.variable_definitions.set(id, defining_block);
        this.variable_dependencies.set(id, new Set());
        return id;
    }

    public register_dependency(consumer_block: any, dependency_var_id: number): void {
        const dependencies = this.variable_dependencies.get(dependency_var_id);
        if (dependencies) {
            dependencies.add(consumer_block);
        }
    }

    public is_variable_defined(var_id: number): boolean {
        return this.variable_definitions.has(var_id);
    }

    public get_consumer_count(var_id: number): number {
        return this.variable_dependencies.get(var_id)?.size ?? 0;
    }

    private is_pattern_valid(pattern: PatternType): boolean
    {
        return false;
    };

    private collapse_pattern(pattern: PatternType): PatternType 
    { 
        /* 
            A collapsable patterns are strictly those patterns which logic withing the bounds
            of pattern.ts expressivness allow to compress that logic by reducing the amount of grammar 
            expression nodes.
        */
        let collapsed_pattern!: PatternType;

        return collapsed_pattern;
    };

    public emit_ir_from_pattern(root_pattern: PatternType)
    {
        if (!this.is_pattern_valid(root_pattern))
        {
            throw new Error("Invalid root pattern");
        };

        const collapsed_root_pattern = this.collapse_pattern(root_pattern);
    };
}

const build_context: BuildContext = new BuildContext();

build_context.emit_ir_from_pattern(PatternsConfig.Patterns.Root);

