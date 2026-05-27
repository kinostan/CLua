import { CppEmitContext } from "#common/emitter";

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

export type VarDefinition = IRTokenAssign;

export type IRBlockType = IRTokenAssign | IRVarReference;

interface BuildContext {
    get_variable_name_from_reference_id(id: number): string;
    get_new_variable_id(type: string): number;
    get_ir_nodes_with_variable_id(id: number): Array<IRBlock<IRBlockType>>;
};

abstract class IRBlock<SubclassType> {
    public class_name: string;

    constructor() {
        this.class_name = this.constructor.name;
    };

    abstract emit(build_context: BuildContext,emitter: CppEmitContext): void;
    abstract process_step(build_context: BuildContext): boolean;
}

export class IRTokenAssign extends IRBlock<IRTokenAssign> {
    public token_var_id: number = -1;
    public token_type: TokenType;

    constructor(token_type: TokenType) {
        super();
        this.token_type = token_type;
    }

    emit(build_context: BuildContext,emitter: CppEmitContext)
    {
        
    };  

    process_step(build_context: BuildContext): boolean {
        if (this.token_var_id == -1)
        {
            this.token_var_id = build_context.get_new_variable_id(this.constructor.name);
        };
        return false;
    }
}

class IRVarReference extends IRBlock<IRVarReference> {
    var_id: number = 0;
    var_definition: VarDefinition;

    constructor(var_definition: VarDefinition) {
        super();
        this.var_definition = var_definition; 
    }
}