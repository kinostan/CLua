"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.CppEmitContext = void 0;
class CppEmitContext {
    constructor() {
        this.lines = [];
        this.indent_level = 0;
        this.indent_text = '    ';
    }
    // -------------------------------------------------------------------------
    // Core Low-Level State Accessors
    // -------------------------------------------------------------------------
    emit_line(line) {
        this.lines.push(this.indent_text.repeat(this.indent_level) + line);
    }
    emit_blank_line() {
        this.lines.push('');
    }
    indent() {
        this.indent_level += 1;
    }
    dedent() {
        if (this.indent_level > 0) {
            this.indent_level -= 1;
        }
    }
    get_result() {
        return this.lines.join('\n');
    }
    // -------------------------------------------------------------------------
    // Specialized Structural Emitters (Directly on Context)
    // -------------------------------------------------------------------------
    emit_block(header, body, suffix = '') {
        this.emit_line(header + ' {');
        this.indent();
        body();
        this.dedent();
        this.emit_line('}' + suffix);
    }
    emit_namespace(name, body) {
        this.emit_block(`namespace ${name}`, body);
    }
    emit_class(name, body, kind = 'class') {
        this.emit_block(`${kind} ${name}`, body, ';');
    }
    emit_function(signature, body) {
        this.emit_block(signature, body);
    }
    emit_if(condition, then_body, else_body, branch_hint) {
        const hint = branch_hint ? ` [[${branch_hint}]]` : '';
        this.emit_line(`if (${condition})${hint}`);
        this.emit_block('', then_body); // Empty header tricks it into clean braces
        if (else_body) {
            // Adjust line back up for clean cuddle if desired, or keep it simple:
            this.emit_line('else');
            this.emit_block('', else_body);
        }
    }
    emit_while(condition, body) {
        this.emit_line(`while (${condition})`);
        this.emit_block('', body);
    }
    emit_enum(name, members) {
        this.emit_block(`enum class ${name}`, () => {
            for (let i = 0; i < members.length; i += 1) {
                const comma = i + 1 < members.length ? ',' : '';
                this.emit_line(`${members[i]}${comma}`);
            }
        }, ';');
    }
}
exports.CppEmitContext = CppEmitContext;
