"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.BaseEmitter = void 0;
const types_1 = require("./clua/types");
class BaseEmitter {
    constructor() {
        this.code = "";
        this.indent = 0;
        this.indent_base_unit = "    "; // 4 spaces
        this.indent_text = "";
        this.code = "";
    }
    emit_class(class_object) {
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
    emit_enum(enum_class_object) {
        this.emit(`enum class ${enum_class_object.enum_name} {`);
        this.step_indent();
        let size_of = enum_class_object.get_enum_size();
        (0, types_1.set_field_type)(enum_class_object.enum_name, size_of);
        for (let index = 0; index < enum_class_object.enum_list.length; index++) {
            const element_name = enum_class_object.enum_list[index];
            const comma_postfix = (index + 1 < enum_class_object.enum_list.length) ? `,` : ``;
            this.emit(`${element_name}${comma_postfix}`);
        }
        this.step_dedent();
        this.emit(`};`);
    }
    // Executes an isolated lexical block wrapper that guarantees proper structural indentation
    emit_scope(lambda) {
        this.emit("{");
        this.step_indent();
        lambda();
        this.step_dedent();
        this.emit("}");
    }
    emit(emit_code) {
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
        // FIXED: Replaced " " with this.indent_base_unit to keep formatting unified
        this.indent_text = this.indent_base_unit.repeat(this.indent);
    }
    // =========================================================================
    // ParserContext Mirror API & Stylers (Operating on your ParserContext signature)
    // =========================================================================
    // Styled Pattern: Stylized header block to keep long generated files clean
    emit_section_header(title) {
        this.emit("");
        this.emit("// =========================================================================");
        this.emit("// PATTERN BLOCK: " + title.toUpperCase());
        this.emit("// =========================================================================");
    }
    // Styled Pattern: An inverted early return guard condition
    emit_guard_clause(cpp_condition, failure_lambda) {
        this.emit(`if (!(${cpp_condition})) `);
        this.emit_scope(() => {
            failure_lambda();
        });
        this.emit(""); // Implicit clean trailing line break
    }
    // Styled Pattern: Surgical context snapshotting for ambiguous grammar regions
    emit_speculative_block(failed_flag_var, match_lambda) {
        this.emit(`// Speculative parse region for non-deterministic grammar node`);
        this.emit(`auto snapshot_state = parser_context.record_cursor();`);
        // Execute inner parsing steps
        match_lambda();
        this.emit(`if (${failed_flag_var}) `);
        this.emit_scope(() => {
            this.emit(`parser_context.set_cursor(snapshot_state);`);
        });
    }
    emit_token_peek_with_error_guard(token_var, parser_error_var, lexer_error_var) {
        this.emit(`auto ${token_var} = parser_context.see_current_token();\n`);
        this.emit(`if (${token_var}.token_type == TokenType::Error) [[unlikely]]`);
        this.emit_scope(() => {
            this.emit(`ParserError ${parser_error_var} = ParserError(${token_var}, ${token_var});`);
            this.emit(`auto ${lexer_error_var} = parser_context.get_current_error();`);
            this.emit(`${parser_error_var}.node_handle = parser_context.create_node<CLuaNodes::LexerErrorNode>(${token_var}, ${lexer_error_var});`);
            this.emit(`return parser_context.emit_error(${parser_error_var});`);
        });
    }
    is_symbol(symbol_kind) {
        return `parser_context.is_symbol(${symbol_kind})`;
    }
    is_identifier() {
        return `parser_context.is_identifier()`;
    }
    get_next_token() {
        this.emit(`parser_context.get_next_token();`);
    }
    see_current_token(variable_name) {
        this.emit(`auto ${variable_name} = parser_context.see_current_token();`);
    }
    // Universal generic node builder
    create_node(node_type, ...args) {
        return `parser_context.create_node<${node_type}>(${args.join(", ")})`;
    }
    // HIGH-FIDELITY DIAGNOSTIC EMITTER: Assembles your exact C++ error setup workflow
    emit_diagnostic_block(error_node_type, start_tok, end_tok, span_tok) {
        this.emit(`ParserError parser_error = ParserError(${start_tok}, ${end_tok});`);
        this.emit(`TokenSpan fake_span = TokenSpan(${span_tok}, ${span_tok});`);
        this.emit(`parser_error.node_handle = ${this.create_node(error_node_type, "fake_span")};`);
        this.emit(`return parser_context.emit_error(parser_error);`);
    }
}
exports.BaseEmitter = BaseEmitter;
