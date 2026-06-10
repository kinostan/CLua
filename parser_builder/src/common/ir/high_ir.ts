import { BuildContext, IRBlock } from "./low_ir"
import { Field } from "#common/node";
import { Pattern } from "#common/pattern";

export type IRBlockType = LogicalConstructs | ActionConstructs;



export class IRRoot extends IRBlock{
    constructor()
    {
        super(IRRoot)
    };

    process_step(build_context: BuildContext): boolean {
        return false;
    }  
};

export class IRParseFunction extends IRBlock
{
    linked_pattern: Pattern

    constructor(function_body: Pattern)
    {
        super(IRParseFunction);
        this.linked_pattern = function_body;
    }

    process_step(build_context: BuildContext): boolean {
        return false;
    };
};

//IR VAR DEFINITION
export namespace VarDefinition {
    class VarDeclaration<T> extends IRBlock{
        var_id: number = -1;

        constructor() {
            super();
        };

        process_step(build_context: BuildContext): boolean {
            return false;
        }
    }

    export class IRDeclNodeHandle extends VarDeclaration<IRDeclNodeHandle> {
        constructor()
        {
            super();
        };

        process_step(build_context: BuildContext): boolean {
            return false;
        }
    };

    //based on auto current_token = parser_context.see_current_token();
    export class IRDefTokenPeek extends VarDeclaration<IRDefTokenPeek> {
        process_step(build_context: BuildContext): boolean {
            /*    
                I have to figure out what kind of logic can be added here for optimization or if 
                process_step should even exist
            */
            return false;
        }
    };

    /* 
        auto error_node = parser_context.create_node<UnexpectedTokenError>(current_token);
    */
    export class IRDefCreateNode extends VarDeclaration<IRDefCreateNode> {
        node_id: NodeReference = -1;
        arguments: Array<VarReference> = new Array<VarReference>();

        insert_variable(variable_reference: VarReference){
            this.arguments.push(variable_reference);
        };

        process_step(build_context: BuildContext): boolean {
            /* 
                Try to confirm if the references do exist and create
                an error if they ever stop existing
            */
            return false;
        }
    };

    export class IRDefParserError extends VarDeclaration<IRDefParserError> {
        error_id: ErrorReference = -1;

        process_step(build_context: BuildContext): boolean {
            return false;
        }
    };
};

export namespace HighIR {
    export type HIRBlockType = IRParsingFunctionDeclaration;
};
