import "#root/common/pattern";

import "#config/pattern";
import "#config/nodes";

import { ParserEmitter } from "#root/common/emitter";
import { ClassDescription } from "#root/common/descriptions";

function emit_parser_cpp()
{
    const emitter = new ParserEmitter();

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
