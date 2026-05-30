import * as NodesConfig from "#root/config/clua/nodes_declare"
import * as PatternsConfig from "#config/clua/patterns"

import { Pattern, PatternType } from "#common/pattern";

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

    public emit_ir_from_pattern(root_pattern: PatternType)
    {

    };
}

const build_context: BuildContext = new BuildContext();

build_context.emit_ir_from_pattern(PatternsConfig.Patterns.Root);

