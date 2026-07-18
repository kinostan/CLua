export class CppEmitContext {
    private readonly lines: string[] = [];
    private indent_level = 0;
    private readonly indent_text = '    ';
    private current_line = ''; // Buffers inline text before a newline is forced

    // -------------------------------------------------------------------------
    // Core Low-Level State Accessors & Inline Writing
    // -------------------------------------------------------------------------
    /** Appends text directly to the current line without adding a newline. */
    emit_text(text: string): void {
        if (this.current_line === '') {
            this.current_line = this.indent_text.repeat(this.indent_level);
        }
        this.current_line += text;
    }

    /** Forces a newline, committing whatever is in the inline buffer. */
    emit_newline(): void {
        if (this.current_line !== '') {
            this.lines.push(this.current_line);
            this.current_line = '';
        } else {
            this.lines.push('');
        }
    }

    /** Emits a complete line. Commits any existing buffered text first. */
    emit_line(line: string): void {
        if (this.current_line !== '') {
            this.emit_newline();
        }
        this.lines.push(this.indent_text.repeat(this.indent_level) + line);
    }

    emit_blank_line(): void {
        this.emit_line('');
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
        if (this.current_line !== '') {
            this.emit_newline();
        }
        return this.lines.join('\n');
    }

    // -------------------------------------------------------------------------
    // Expression & Declaration Emitters
    // -------------------------------------------------------------------------
    /** Emits an expression terminated by a semicolon. */
    emit_statement(expr: string): void {
        this.emit_line(`${expr};`);
    }

    /** Emits a variable declaration or assignment: type name = value; */
    emit_declaration(type: string, name: string, value?: string): void {
        if (value !== undefined) {
            this.emit_line(`${type} ${name} = ${value};`);
        } else {
            this.emit_line(`${type} ${name};`);
        }
    }

    /** Emits an assignment expression: target = value; */
    emit_assignment(target: string, value: string): void {
        this.emit_line(`${target} = ${value};`);
    }

    // -------------------------------------------------------------------------
    // Specialized Structural Emitters
    // -------------------------------------------------------------------------
    emit_block(header: string | (() => void), body: () => void, suffix: string = ''): void {
        if (typeof header === 'function') {
            header();
            this.emit_text(' {');
            this.emit_newline();
        } else {
            const prefix = header ? `${header} {` : '{';
            this.emit_line(prefix);
        }

        this.indent();
        body();
        this.dedent();
        this.emit_line(`}${suffix}`);
    }

    emit_if(
        condition: string | (() => void), 
        then_body: () => void, 
        else_body?: () => void, 
        branch_hint?: 'likely' | 'unlikely'
    ): void {
        const hint = branch_hint ? ` [[${branch_hint}]]` : '';
        
        const header_callback = () => {
            this.emit_text('if (');
            if (typeof condition === 'function') {
                condition();
            } else {
                this.emit_text(condition);
            }
            this.emit_text(`)${hint}`);
        };

        this.emit_block(header_callback, then_body);
        
        if (else_body) {
            this.emit_block('else', else_body);
        }
    }

    emit_while(condition: (() => void), body: () => void): void {
        const header_callback = () => {
            this.emit_text('while (');
            condition();
            this.emit_text(')');
        };

        this.emit_block(header_callback, body);
    }

    emit_include(include_path: string): void {
        this.emit_line(`#include ${include_path}`);
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
}