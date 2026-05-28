import { CppEmitContext } from "#common/emitter";
import { Field } from "./node";

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

// Wszystko co definiuje nową zmienną w rejestrze IR parsera
export type VarDefinition = IRTokenAssign | IRInvokeParserRule | IRFetchLexerValue;

export type IRBlockType = 
    | IRTokenAssign 
    | IRInvokeParserRule 
    | IRFetchLexerValue
    | IRVarReference 
    | IRBindField 
    | IRCreateASTNode
    | IRScope;

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

    abstract emit: (build_context: BuildContext, emitter: CppEmitContext) => void;
    abstract process_step(build_context: BuildContext): boolean;
}

// ==========================================
// 1. BLOKI AKCJI STRUMIENIA I EKSTRAKCJI
// ==========================================

export class IRTokenAssign extends IRBlock<IRTokenAssign> {
    public token_var_id: number = -1;
    public token_type: TokenType;

    constructor(token_type: TokenType) {
        super();
        this.token_type = token_type;
    }

    process_step(build_context: BuildContext): boolean {
        if (this.token_var_id === -1) {
            this.token_var_id = build_context.get_new_variable_id(this.constructor.name);
        }
        return false;
    }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {
        const var_name = `var_${this.token_var_id}`;
        emitter.emit_line(`auto ${var_name} = parser_context.see_current_token().as<CLua::${this.token_type}>();`);
        emitter.emit_line(`parser_context.get_next_token();`);
    };
}

export class IRInvokeParserRule extends IRBlock<IRInvokeParserRule> {
    public rule_var_id: number = -1;
    
    constructor(public readonly rule_name: string) {
        super();
    }

    process_step(build_context: BuildContext): boolean {
        if (this.rule_var_id === -1) {
            this.rule_var_id = build_context.get_new_variable_id(this.constructor.name);
        }
        return false;
    }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {
        const var_name = `var_${this.rule_var_id}`;
        emitter.emit_line(`auto ${var_name} = Expression::expect_${this.rule_name}(parser_context);`);
    };
}

export class IRFetchLexerValue extends IRBlock<IRFetchLexerValue> {
    public value_var_id: number = -1;

    constructor(public readonly value_kind: LexerValueKind) {
        super();
    }

    process_step(build_context: BuildContext): boolean {
        if (this.value_var_id === -1) {
            this.value_var_id = build_context.get_new_variable_id(this.constructor.name);
        }
        return false;
    }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {
        const var_name = `var_${this.value_var_id}`;
        
        switch (this.value_kind) {
            case "Symbol":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_symbol();`);
                break;
            case "Keyword":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_keyword();`);
                break;
            case "Char":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_char_value();`);
                break;
            case "Integer":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_integer();`);
                break;
            case "Fraction":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_fraction();`);
                break;
            case "NumberHint":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_number_hint();`);
                break;
            case "Error":
                emitter.emit_line(`auto ${var_name} = parser_context.get_current_error();`);
                break;
        }
    };
}

// ==========================================
// 2. BLOKI STRUKTURALNE I REFERENCJE
// ==========================================

export class IRVarReference extends IRBlock<IRVarReference> {
    public var_id: number = -1;
    public var_definition: VarDefinition;

    constructor(var_definition: VarDefinition) {
        super();
        this.var_definition = var_definition;
        this.sync_id();
    }

    private sync_id(): void {
        if (this.var_definition instanceof IRTokenAssign) this.var_id = this.var_definition.token_var_id;
        else if (this.var_definition instanceof IRInvokeParserRule) this.var_id = this.var_definition.rule_var_id;
        else if (this.var_definition instanceof IRFetchLexerValue) this.var_id = this.var_definition.value_var_id;
    }

    process_step(build_context: BuildContext): boolean {
        this.sync_id();
        return false;
    }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {};
}

export class IRBindField extends IRBlock<IRBindField> {
    constructor(
        public readonly target_field: Field,
        public readonly source_var: VarDefinition
    ) {
        super();
    }

    process_step(build_context: BuildContext): boolean { return false; }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {
        let source_id = -1;
        if (this.source_var instanceof IRTokenAssign) source_id = this.source_var.token_var_id;
        else if (this.source_var instanceof IRInvokeParserRule) source_id = this.source_var.rule_var_id;
        else if (this.source_var instanceof IRFetchLexerValue) source_id = this.source_var.value_var_id;

        emitter.emit_line(`// Mapping register var_${source_id} to node field: ${this.target_field.identifier}`);
    };
}

export class IRCreateASTNode extends IRBlock<IRCreateASTNode> {
    constructor(
        public readonly node_name: string,
        public readonly bound_fields: Array<IRBindField>
    ) {
        super();
    }

    process_step(build_context: BuildContext): boolean { return false; }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {
        const args = this.bound_fields.map(bf => {
            let source_id = -1;
            if (bf.source_var instanceof IRTokenAssign) source_id = bf.source_var.token_var_id;
            else if (bf.source_var instanceof IRInvokeParserRule) source_id = bf.source_var.rule_var_id;
            else if (bf.source_var instanceof IRFetchLexerValue) source_id = bf.source_var.value_var_id;
            return `var_${source_id}`;
        }).join(", ");

        emitter.emit_line(`auto result_node = parser_context.create_node<${this.node_name}>(${args});`);
        emitter.emit_line(`return result_node;`);
    };
}

// ==========================================
// 3. LOGICZNE KONTENERY ZAKRESU (Scope Boundary)
// ==========================================

export class IRScope extends IRBlock<IRScope> {
    public readonly ir_nodes: Array<IRBlockType> = new Array<IRBlockType>();

    constructor(
        public readonly scope_name: string,
        public readonly is_function: boolean = false
    ) {
        super();
    }

    public insert_ir(ir_block: IRBlockType): void {
        this.ir_nodes.push(ir_block);
    }

    public process_step(build_context: BuildContext): boolean {
        let changed = false;
        this.ir_nodes.forEach(node => {
            if (node instanceof IRBlock && node.process_step(build_context)) {
                changed = true;
            }
        });
        return changed;
    }

    public emit = (build_context: BuildContext, emitter: CppEmitContext): void => {
        const execute_body = () => {
            this.ir_nodes.forEach(element => {
                if (element instanceof IRBlock) {
                    element.emit(build_context, emitter);
                }
            });
        };

        if (this.is_function) {
            emitter.emit_function(`NodeHandle expect_${this.scope_name}(ParserContext& parser_context)`, execute_body);
        } else {
            emitter.emit_namespace(`namespace ${this.scope_name}`,execute_body);
        }
    };
}