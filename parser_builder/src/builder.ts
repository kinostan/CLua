import "./clua/pattern";

import "./config/pattern";
import "./config/nodes";

import { BaseEmitter } from "./emitter";
import { ClassDescription } from "./clua/descriptions";

function emit_parser_cpp()
{
    let class_desc: ClassDescription = new ClassDescription(
        "eh", null, 
        [
            ["field1", "Common::uint8"],
            ["field2", "CLua::NodeHandle"],
            ["field3", "CLua::NodeHandle"]
        ]
    );

    let emitter: BaseEmitter = new BaseEmitter();
    emitter.emit_class(class_desc);

    console.log(emitter.code);
};

function emit_ast_nodes()
{

};

export function build_parser()
{
    //process nodes and then pattern, then emit the code
    emit_parser_cpp();
};
