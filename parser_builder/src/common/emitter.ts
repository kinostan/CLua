import { EnumClassDescription, ClassDescription, NamespaceDescription } from "#root/common/descriptions"
import { set_field_type, Type } from "#root/common/parser-types"

export class BaseEmitter {
    code: string = "";
    indent: number = 0;
    indent_base_unit: string = "    "; 
    indent_text: string = "";

    constructor() {
        this.code = "";
    }

    emit(emit_code: string) {
        this.code += this.indent_text + emit_code + '\n';
    }

    step_indent() {
        this.indent++;
        this.indent_text = this.indent_base_unit.repeat(this.indent);
    }

    step_dedent() {
        this.indent--;
        if (this.indent < 0) {
            throw new Error("indent is less than 0, which is illegal I guess...");
        }
        this.indent_text = this.indent_base_unit.repeat(this.indent);
    }

    emit_class(class_object: ClassDescription) {
        let inheritance = class_object.inherited_class ? `: public ${class_object.inherited_class}` : ``;

        this.emit(`class ${class_object.class_name}${inheritance} {`);
        this.step_indent();
        this.emit(`public:`);

        let field_list = class_object.get_sorted_fields();
        for (const field_element of field_list) {
            this.emit(`${field_element.type} ${field_element.name} = ${field_element.type}{0};`);
        }

        this.step_dedent();
        this.emit(`};`);
    }

    emit_enum(enum_class_object: EnumClassDescription) {
        this.emit(`enum class ${enum_class_object.enum_name} {`);
        this.step_indent();

        let size_of = enum_class_object.get_enum_size();
        
        set_field_type(enum_class_object.get_enum_size_unit(), size_of);

        for (let index = 0; index < enum_class_object.enum_list.length; index++) {
            const element_name = enum_class_object.enum_list[index];
            const comma_postfix = (index + 1 < enum_class_object.enum_list.length) ? `,` : ``;
            this.emit(`${element_name}${comma_postfix}`);
        }

        this.step_dedent();
        this.emit(`};`);
    }

    emit_scope(lambda: () => void) {
        this.emit("{");
        this.step_indent();
        lambda();
        this.step_dedent();
        this.emit("}");
    }

    emit_namespace(namespace_object: NamespaceDescription, lambda: () => void)
    {
        this.emit(`namespace ${namespace_object.name} {`);
        this.step_indent();
        lambda();
        this.step_dedent();
        this.emit(`}`);
    }

    emit_function(return_type: Type, function_name: string, arg_list: [Type, string][], lambda: () => void) {
        const formatted_args = arg_list.map(([type, name]) => `${type} ${name}`).join(", ");

        this.emit(`${return_type} ${function_name}(${formatted_args}) {`);
        this.step_indent();
        lambda();
        this.step_dedent();
        
        this.emit(`}`); 
    }

    // =========================================================================
    // ParserContext Mirror API & Stylers (Operating on your ParserContext signature)
    // =========================================================================

    // Styled Pattern: Stylized header block to keep long generated files clean
    emit_section_header(title: string) {
        this.emit("");
        this.emit("// =========================================================================");
        this.emit("// PATTERN BLOCK: " + title.toUpperCase());
        this.emit("// =========================================================================");
    }

    // Styled Pattern: An inverted early return guard condition
    emit_guard_clause(cpp_condition: string, failure_lambda: () => void) {
        this.emit(`if (!(${cpp_condition})) `);
        this.emit_scope(() => {
            failure_lambda();
        });
        this.emit(""); // Implicit clean trailing line break
    }

    // Styled Pattern: Surgical context snapshotting for ambiguous grammar regions
    emit_speculative_block(failed_flag_var: string, match_lambda: () => void) {
        this.emit(`// Speculative parse region for non-deterministic grammar node`);
        this.emit(`auto snapshot_state = parser_context.record_cursor();`);
        
        // Execute inner parsing steps
        match_lambda();

        this.emit(`if (${failed_flag_var}) `);
        this.emit_scope(() => {
            this.emit(`parser_context.set_cursor(snapshot_state);`);
        });
    }
    
    emit_token_peek_with_error_guard(token_var: string, parser_error_var: string, lexer_error_var: string) {
        this.emit(`auto ${token_var} = parser_context.see_current_token();\n`);
 
        this.emit(`if (${token_var}.token_type == TokenType::Error) [[unlikely]]`);
        this.emit_scope(() => {
            this.emit(`ParserError ${parser_error_var} = ParserError(${token_var}, ${token_var});`);
            this.emit(`auto ${lexer_error_var} = parser_context.get_current_error();`);
            this.emit(`${parser_error_var}.node_handle = parser_context.create_node<CLuaNodes::LexerErrorNode>(${token_var}, ${lexer_error_var});`);
            this.emit(`return parser_context.emit_error(${parser_error_var});`);
        });
    }

    is_symbol(symbol_kind: string): string {
        return `parser_context.is_symbol(${symbol_kind})`;
    }

    is_identifier(): string {
        return `parser_context.is_identifier()`;
    }

    get_next_token() {
        this.emit(`parser_context.get_next_token();`);
    }

    see_current_token(variable_name: string) {
        this.emit(`auto ${variable_name} = parser_context.see_current_token();`);
    }
}

export class ParserEmitter extends BaseEmitter {

    constructor()
    {
        super();
    };

       // Universal generic node builder
    create_node(node_type: string, ...args: string[]): string {
        return `parser_context.create_node<${node_type}>(${args.join(", ")})`;
    }

    // HIGH-FIDELITY DIAGNOSTIC EMITTER: Assembles your exact C++ error setup workflow
    emit_diagnostic_block(error_node_type: string, start_tok: string, end_tok: string,) {
        this.emit(`ParserError parser_error = ParserError(${start_tok}, ${end_tok});`);
        this.emit(`TokenSpan fake_span = TokenSpan(${start_tok}, ${end_tok});`);
        this.emit(`parser_error.node_handle = ${this.create_node(error_node_type, "fake_span")};`);
        this.emit(`return parser_context.emit_error(parser_error);`);
    }

    // -------------------------------------------------------------------------
    // Small building-block emitters (compose these to craft larger functions)
    // -------------------------------------------------------------------------

    /** Emit an if that checks a symbol and emits the provided body. */
    emit_if_symbol(symbol_kind: string, body: () => void) {
        this.emit(`if (${this.is_symbol(symbol_kind)}) `);
        this.emit_scope(body);
    }

    /** Emit an if that checks identifier predicate and emits body. */
    emit_if_identifier(body: () => void) {
        this.emit(`if (${this.is_identifier()}) `);
        this.emit_scope(body);
    }

    /** Emit assignment that creates a node and stores it in a local var. */
    emit_create_node_assign(var_name: string, node_type: string, ...args: string[]) {
        const call = `${this.create_node(node_type, ...args)}`;
        this.emit(`auto ${var_name} = ${call};`);
    }

    /** Emit a simple return statement. */
    emit_return(var_name?: string) {
        if (var_name) this.emit(`return ${var_name};`);
        else this.emit(`return;`);
    }

    /** Emit a commented list of node field descriptions for later consumption. */
    emit_node_field_list(node_defs: Array<{ name: string; fields: Array<{ name: string; type: string }> }>) {
        this.emit_section_header("node_field_list");
        for (const node of node_defs) {
            this.emit(`// Node: ${node.name}`);
            for (const f of node.fields) {
                this.emit(`//   - ${f.name} : ${f.type}`);
            }
            this.emit(``);
        }
    }

    emit_parser_function(function_name: string, body_lambda: () => void) {
        this.emit_function("NodeHandle", function_name, [["ParserContext&", "parser_context"]], body_lambda);
    };
}