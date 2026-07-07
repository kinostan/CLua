export class CppEmitContext {
    private readonly lines: string[] = [];
    private indent_level = 0;
    private readonly indent_text = '    ';

    // -------------------------------------------------------------------------
    // Core Low-Level State Accessors
    // -------------------------------------------------------------------------
    emit_line(line: string): void {
        this.lines.push(this.indent_text.repeat(this.indent_level) + line);
    }

    emit_blank_line(): void {
        this.lines.push('');
    }

    indent(): void {
        this.indent_level += 1;
    }

    dedent(): void {
        if (this.indent_level > 0) {
            this.indent_level -= 1;
        }
    }

    get_result(): string {
        return this.lines.join('\n');
    }

    // -------------------------------------------------------------------------
    // Specialized Structural Emitters (Directly on Context)
    // -------------------------------------------------------------------------
    emit_block(header: string, body: () => void, suffix: string = ''): void {
        this.emit_line(header + ' {');
        this.indent();
        body();
        this.dedent();
        this.emit_line('}' + suffix);
    }

    emit_namespace(name: string, body: () => void): void {
        this.emit_block(`namespace ${name}`, body);
    }

    emit_class(name: string, body: () => void, kind: 'class' | 'struct' = 'class'): void {
        this.emit_block(`${kind} ${name}`, body, ';');
    }

    emit_function(signature: string, body: () => void): void {
        this.emit_block(signature, body);
    }

    emit_if(condition: string, then_body: () => void, else_body?: () => void, branch_hint?: 'likely' | 'unlikely'): void {
        const hint = branch_hint ? ` [[${branch_hint}]]` : '';
        this.emit_line(`if (${condition})${hint}`);
        this.emit_block('', then_body); // Empty header tricks it into clean braces
        
        if (else_body) {
            // Adjust line back up for clean cuddle if desired, or keep it simple:
            this.emit_line('else');
            this.emit_block('', else_body);
        }
    }

    emit_while(condition: string, body: () => void): void {
        this.emit_line(`while (${condition})`);
        this.emit_block('', body);
    }

    emit_enum(name: string, members: string[]): void {
        this.emit_block(`enum class ${name}`, () => {
            for (let i = 0; i < members.length; i += 1) {
                const comma = i + 1 < members.length ? ',' : '';
                this.emit_line(`${members[i]}${comma}`);
            }
        }, ';');
    }

    emit_include(include_path: string)
    {
        this.emit_line(`#include ${include_path}`);
    };
}