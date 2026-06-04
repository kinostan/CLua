import * as NodesConfig from "#root/config/clua/nodes_declare"
import * as PatternsConfig from "#config/clua/patterns"

import { ChoicePattern, MatchCharToken, MatchIdentifierToken, MatchKeywordToken, MatchNumericToken, MatchStringToken, MatchSymbolToken, Pattern, PatternOperators, PatternSwitchParser, PatternType, QuantityPattern } from "#common/pattern";
import { IRBlockType, IRParsingFunctionCall, IRParsingFunctionDefinition, IRRoot, VarDefinition } from "./common/ir_parser_blocks";
import { CppEmitContext } from "./common/emitter";
import { Field, NodeDefinition } from "./common/node";
import { NodeRegistry } from "./config/clua/nodes_define";

class NodeInitializationContext {
    node_ir: VarDefinition.IRDefCreateNode;
    node_definition: NodeDefinition
    current_field: number = 0;
    variable_id: number = -1;
    
    constructor(node_id: number)
    {   
        if (node_id == -1)
        {
            throw new Error(`Can't initialize node context without node_id is -1`);
        };

        if (!NodeRegistry.has(node_id)) {
            throw new Error(`couldn't find the node of the given id: ${node_id}`);
        }

        this.node_ir = new VarDefinition.IRDefCreateNode();
        this.node_definition = NodeRegistry.get(node_id)!;
    };

    link_as_variable(build_context: BuildContext)
    {
        this.variable_id = build_context.allocate_var_id(this.node_ir);
    };

    does_field_match(field: PatternType)
    {
        if (this.current_field >= this.node_definition.fields.length)
        {
            console.warn("Redundant definition in grammar, those next tokens might not be needed");
            return false;
        };

        return this.node_definition.fields[this.current_field]!.field_pattern.class_name == field.class_name;
    };

    insert_variable()
    {
        this.current_field++;
    };
};

/*
NodeArray (basically buffer with a type and the value of NodeArray is basically a virtual index 
pointing to a real index within the memory manager) 
should be added as a type to the parser.hpp when 
memory manager gets advanced enough as it fits very well for 
QuantityPattern and reduces memory costs when there's lots of nodes
*/
export class BuildContext implements BuildContext {
    private next_var_id: number = 0;
    private ir_tree_root!: IRBlockType;

    private variable_definitions = new Map<number, IRBlockType>();
    private variable_dependencies = new Map<number, Set<IRBlockType>>();
    private pattern_map: Map<Pattern, boolean> = new Map<Pattern, boolean>();

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

        this.pattern_map.forEach((is_enabled, pattern) => {
            if (!is_enabled) {
                throw new Error(`unexpected behaviour: disabled pattern in tracking map`);
            }
            
            const function_ir = this.transform_pattern_to_function_definition(pattern);
            root.insert_child(function_ir);
        });
    }

    private transform_pattern_to_function_definition(pattern: Pattern): IRParsingFunctionDefinition {
        const function_def = new IRParsingFunctionDefinition(pattern);

        let node_definition: NodeDefinition | null = null;
        let current_node_field = 0;

        let error_code = pattern.error;

        let node_ir = new VarDefinition.IRDefCreateNode();

        if (pattern.node_id !== -1) {
            
            const assigned_id = this.allocate_var_id(node_ir);
            node_ir.insert_variable(assigned_id);
        }

        function get_current_field_of_node(): Field
        {
            if (!node_definition || pattern.node_id == -1)
            {
                throw new Error("Tried to call get_current_field_of_node() function when node_definition doesn't exist");
            };

            if (current_node_field >= node_definition.fields.length)
            {
                throw new Error("current_node_field is reading outside of node_definition fields");
            };

            const node_field = node_definition.fields[current_node_field];

            if (!node_field)
            {
                throw new Error("Unexpected program behvaiour: node_field is undefined");
            };

            return node_field;
        };

        pattern.get_children().forEach((child_pattern: PatternType) => {
           
        });

        // Insert completed structural instruction block into definition list
        function_def.insert_child(node_ir);
        return function_def;
    }

    private transform_choice_pattern_to_choice_ir(node_init_context: NodeInitializationContext)
    {
        
    };

    /*
    public optimize_ir(): void {
        while (this.ir_tree_root.process_step(this)) {};
    }
    */

    public pretty_print_ir(): void {}
    public emit_parser_code(): void {}
}

const build_context: BuildContext = new BuildContext();

build_context.generate_ir_from_pattern(PatternsConfig.Patterns.Root);

