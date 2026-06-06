import { CppEmitContext } from "#common/emitter";
import { Pattern } from "#common/pattern";

export interface BuildContext {
    get_variable_name_from_reference_id(id: number): string;
    get_new_variable_id(type: string): number;
    get_ir_nodes_with_variable_id(id: number): Array<IRBlock>;
}

export abstract class IRBlock {
    public is_dead: boolean = false;
    public readonly real_constructor: Function = this.constructor;

    private child_elements: Array<IRBlock>;
    parent: IRBlock | null = null;

    constructor(constructor: Function) {
        this.real_constructor = constructor;
        this.child_elements = new Array<IRBlock>()
    }

    abstract process_step(build_context: BuildContext): boolean;
    
    append_child(ir_child: IRBlock)
    {
        this.child_elements.push(ir_child);
        ir_child.parent = this;
    };

    remove_child(ir_child: IRBlock): void
    {
        const index = this.child_elements.indexOf(ir_child);
        
        if (index !== -1) {
            this.child_elements.splice(index, 1);
            ir_child.parent = null;
        }
    };

    remove(): void
    {
        if (this.parent == null)
        {
            return;
        };
        this.parent.remove_child(this);
    };
    
    get_children(): Array<IRBlock>
    {
        return this.child_elements.slice();
    };
}

export class IRRoot extends IRBlock{
    parser_name: string = "DefaultParserName";

    constructor(parser_name: string)
    {
        super(IRRoot);
        this.parser_name = parser_name;
    };

    process_step(build_context: BuildContext): boolean {
        return false;
    }  
};

export namespace LowIR {
    export type LIRBlockType = IRParsingFunctionDeclaration;

    export class IRParsingFunctionDeclaration extends IRBlock
    {
        linked_pattern: Pattern;

        constructor(pattern: Pattern)
        {
            super(IRParsingFunctionDeclaration);
            this.linked_pattern = pattern;
        }
    
        process_step(build_context: BuildContext): boolean {
            return false;
        };
    }
    

}

export namespace LIROperators {

    function transform_low_ir_with_dispatch_into_parser_code(emit_context: CppEmitContext, current_ir_block: IRBlock)
    {

    };

    export function transform_low_ir_into_parser_code(emit_context: CppEmitContext, lir_root: IRRoot)
    {
        let RootChildren = lir_root.get_children();

        emit_context.emit_include(`"parser.hpp"`);
        
        emit_context.emit_include(`<common/clua/tokens.hpp>`);
        emit_context.emit_include(`<common/base.hpp>`);

        RootChildren.forEach((ir_block: IRBlock) => {
            transform_low_ir_with_dispatch_into_parser_code(emit_context,ir_block);
        })
    };
};