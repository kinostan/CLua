import { CppEmitContext } from "#common/emitter";
import { Field } from "./node";
import { ErrorType } from "./error";

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

export type IRBlockType = VarDefinition.IRDefCreateNode | VarDefinition.IRDefTokenPeek;

export interface BuildContext {
    get_variable_name_from_reference_id(id: number): string;
    get_new_variable_id(type: string): number;
    get_ir_nodes_with_variable_id(id: number): Array<IRBlock<IRBlockType>>;

    request_removal(block: IRBlockType): void;
    move_block(block: IRBlockType, target_scope: IRScope): void;
    get_parent_scope(block: IRBlockType): IRScope | null;
}

export abstract class IRBlock<SubclassType> {
    public readonly class_name: string;
    public is_dead: boolean = false;

    constructor() {
        this.class_name = this.constructor.name;
    }

    abstract process_step(build_context: BuildContext): boolean;
}

type VarDefinitionType = IRDefCreateNode | IRDefTokenPeek;

//IR STATEMENT

export namespace Statemet {
    class Statemet
    {
        likely: boolean = false;
        ir_expression: IRExpression = null;
    };
};

//IR VAR DEFINITION
export namespace VarDefinition {
    abstract class VarDefinition<T> extends IRBlock<T> {
        var_id: number = -1;

        constructor() {
            super();
        };
    }

    //based on auto current_token = parser_context.see_current_token();
    export class IRDefTokenPeek extends VarDefinition<IRDefTokenPeek> {
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
    export class IRDefCreateNode extends VarDefinition<IRDefCreateNode> {
        node_id: number = -1;
        arguments: Array<VarDefinitionType> = new Array<VarDefinitionType>();

        process_step(build_context: BuildContext): boolean {
            /* 
                Try to confirm if the references do exist and create
                an error if they ever stop existing
            */
            return false;
        }
    };

    export class IRDefParserError extends VarDefinition<IRDefParserError> {

        process_step(build_context: BuildContext): boolean {
            return false;
        }
    };
};

