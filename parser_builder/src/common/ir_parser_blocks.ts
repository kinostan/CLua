import { CppEmitContext } from "#common/emitter";
import { Field } from "./node";
import { Pattern } from "./pattern";

export type TokenType = 
    | "IdentifierToken" 
    | "NumericToken" 
    | "SymbolToken" 
    | "WhitespaceToken" 
    | "CommentToken" 
    | "StringToken" 
    | "CharToken" 
    | "NewLineToken" 
    | "EOFToken" 
    | "ErrorToken" 
    | "NoToken";

export type LexerValueKind = "Symbol" | "Keyword" | "Char" | "Integer" | "Fraction" | "NumberHint" | "Error";

type VarReference = number; 
type NodeReference = number;
type ErrorReference = number;

type VarDefinitionType = 
VarDefinition.IRDefCreateNode | 
VarDefinition.IRDefTokenPeek | 
VarDefinition.IRDefParserError;

type IRFunction = IRParsingFunctionDeclaration | IRParsingFunctionDefinition;

type ActionConstructs = VarDefinitionType | IRParsingFunctionCall;
type LogicalConstructs = IRRoot | IRFunction;

export type IRBlockType = LogicalConstructs | ActionConstructs;

export interface BuildContext {
    get_variable_name_from_reference_id(id: number): string;
    get_new_variable_id(type: string): number;
    get_ir_nodes_with_variable_id(id: number): Array<IRBlock<IRBlockType>>;

    request_removal(block: IRBlockType): void;
    move_block(block: IRBlockType, target_scope: IRParsingFunctionDefinition): void;
    get_parent_scope(block: IRBlockType): IRParsingFunctionDefinition | null;
}

export abstract class IRBlock<SubclassType> {
    public readonly class_name: string;
    public is_dead: boolean = false;

    constructor() {
        this.class_name = this.constructor.name;
    }

    abstract process_step(build_context: BuildContext): boolean;
}

class IRChildElementsComponent<ParentType> extends IRBlock<ParentType>
{
    child_elements: Array<IRBlockType>;

    constructor()
    {
        super();
        this.child_elements = new Array<IRBlockType>()
    } 

    insert_child(ir_child: IRBlockType)
    {
        this.child_elements.push(ir_child);
    };

    get_children()
    {
        return this.child_elements.slice();
    };

    process_step(build_context: BuildContext): boolean {
        return false;
    }
}

export class IRRoot extends IRChildElementsComponent<IRRoot>{
    constructor()
    {
        super()
    };

    process_step(build_context: BuildContext): boolean {
        return false;
    }  
};

export class IRParsingFunctionDeclaration extends IRBlock<IRParsingFunctionDeclaration>
{
    constructor(function_body: Pattern)
    {
        super();
    }

    process_step(build_context: BuildContext): boolean {
        return false;
    };
}

export class IRParsingFunctionDefinition extends IRChildElementsComponent<IRParsingFunctionDefinition>
{
    linked_pattern: Pattern

    constructor(function_body: Pattern)
    {
        super();
        this.linked_pattern = function_body;
    }

    process_step(build_context: BuildContext): boolean {
        return false;
    };
};

export class IRParsingFunctionCall extends IRBlock<IRParsingFunctionCall>
{
    linked_pattern: Pattern

    constructor(function_body: Pattern)
    {
        super();
        this.linked_pattern = function_body;
    }

    process_step(build_context: BuildContext): boolean {
        return false;
    };
}

//IR VAR DEFINITION
export namespace VarDefinition {
    class VarDeclaration<T> extends IRBlock<T> {
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

