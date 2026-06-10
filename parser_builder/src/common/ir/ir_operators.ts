import { CppEmitContext } from "#common/emitter";
import { NumericTokenType, Pattern } from "#common/pattern";
import { IRBlock, IRRoot } from "./low_ir";
import { HighIR } from "./high_ir";

export namespace IROperators {

    function transform_low_ir_with_dispatch_into_parser_code(emit_context: CppEmitContext, current_ir_block: IRBlock)
    {
        let type = current_ir_block.real_constructor;
        switch (type) {
            case value:
                break;
            default:
                throw new Error(`${type.name} is not a handled constructor type`);
        }
    };

    export function transform_low_ir_into_parser_code_cpp(emit_context: CppEmitContext, lir_root: IRRoot)
    {
        let RootChildren = lir_root.get_children();

        emit_context.emit_include(`"parser.hpp"`);
        
        emit_context.emit_include(`<common/clua/tokens.hpp>`);
        emit_context.emit_include(`<common/base.hpp>`);

        RootChildren.forEach((ir_block: IRBlock) => {
            transform_low_ir_with_dispatch_into_parser_code(emit_context,ir_block);
        })
    };

    export function transform_high_ir_into_low_ir(root: IRRoot)
    {
    
    };
};