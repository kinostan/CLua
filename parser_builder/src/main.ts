import * as NodesConfig from "#root/config/clua/nodes_declare"
import * as PatternsConfig from "#config/clua/patterns"

import { ChoicePattern, MatchCharToken, MatchIdentifierToken, MatchKeywordToken, MatchNumericToken, MatchStringToken, MatchSymbolToken, Pattern, PatternOperators, PatternSwitchParser, PatternType, QuantityPattern } from "#common/pattern";
import { IRBlockType, IRParsingFunctionCall, IRParsingFunctionDefinition, IRRoot, VarDefinition } from "./common/ir_parser_blocks";
import { CppEmitContext } from "./common/emitter";
import { NodeDefinition } from "./common/node";
import { NodeRegistry } from "./config/clua/nodes_define";

/*
NodeArray (basically buffer with a type and the value of NodeArray is basically a virtual index 
pointing to a real index within the memory manager) 
should be added as a type to the parser.hpp when 
memory manager gets advanced enough as it fits very well for 
QuantityPattern and reduces memory costs when there's lots of nodes
*/
export class BuildContext implements BuildContext {
    private next_var_id = 0;
    private ir_tree_root!: IRBlockType;

    private variable_definitions = new Map<number, IRBlockType>();
    private variable_dependencies = new Map<number, Set<IRBlockType>>();
    private pattern_map: Map<Pattern, boolean> = new Map<Pattern, boolean>();

    private get_node_definition_from_id(node_id: number): NodeDefinition {
        if (!NodeRegistry.has(node_id)) {
            throw new Error(`couldn't find the node of the given id: ${node_id}`);
        }
        return NodeRegistry.get(node_id)!;
    }

    public allocate_var_id(defining_block: IRBlockType): number {
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

    // ... utility wrappers (is_variable_defined, get_consumer_count) stay clean here ...

    /**
     * PASS 1: High Level Gateway Entry Point
     */
    public generate_ir_from_pattern(root_pattern: Pattern): void {
        const collapsed_root_pattern = PatternOperators.collapse_pattern(root_pattern); 
        const root = new IRRoot();
        this.ir_tree_root = root;

        // Flattened Loop Execution pass
        this.pattern_map.forEach((is_enabled, pattern) => {
            if (!is_enabled) {
                throw new Error(`unexpected behaviour: disabled pattern in tracking map`);
            }
            
            // Delegate the logic directly to a dedicated transformation method
            const function_ir = this.transform_pattern_to_function_definition(pattern);
            root.insert_ir_parser_function(function_ir);
        });
    }

    /**
     * PASS 2: Explicit SSA-Style Transformation Engine
     */
    private transform_pattern_to_function_definition(pattern: Pattern): IRParsingFunctionDefinition {
        const function_def = new IRParsingFunctionDefinition();
        
        // 1. Context Metadata Setup
        let node_definition: NodeDefinition | null = null;
        let current_node_field = 0;
        let max_node_fields = 0;
        let error_code = pattern.error;

        // 2. Safely Instantiate and Register our Target Instruction Node Block
        let node_ir = new VarDefinition.IRDefCreateNode();
        if (pattern.node_id !== -1) {
            node_definition = this.get_node_definition_from_id(pattern.node_id);
            max_node_fields = node_definition.fields.length;
            node_ir.node_id = pattern.node_id;
            
            // Allocate register ID for this allocation
            const assigned_id = this.allocate_var_id(node_ir);
            // node_ir.set_id(assigned_id); // Track register state inside structural blocks
        }

        // 3. Structural Decomposition Block Pass
        pattern.get_children().forEach((child_pattern: PatternType) => {
            // Your clean handlers for Choice/Quantity patterns go here
            // E.g., emitting IRParsingFunctionCall, tracking choice lookaheads, etc.
        });

        // Insert completed structural instruction block into definition list
        function_def.insert_child(node_ir);
        return function_def;
    }

    public optimize_ir(): void {
        while (this.ir_tree_root.process_step(this)) {};
    }

    public pretty_print_ir(): void {}
    public emit_parser_code(): void {}
}

const build_context: BuildContext = new BuildContext();

build_context.generate_ir_from_pattern(PatternsConfig.Patterns.Root);

