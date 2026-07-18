import { IR } from "#common/ir/ir";
import { CppEmitContext } from "#common/emittion/emitter";

type IRBase = IR.IRBase;



function generate_code_from_IR_tree(ir_root: IRBase)
{

};

let global_token = new IR.IdentifierToken("token");

generate_code_from_IR_tree(
    new IR.IRDefinitionStart()
    .insert_child(
        new IR.IRParseFunctionDefinition(global_token)
    )
);
