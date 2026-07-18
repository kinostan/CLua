"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.IRScope = exports.IRCreateASTNode = exports.IRBindField = exports.IRVarReference = exports.IRFetchLexerValue = exports.IRInvokeParserRule = exports.IRTokenAssign = exports.IRBlock = void 0;
class IRBlock {
    constructor() {
        this.is_dead = false;
        this.class_name = this.constructor.name;
    }
}
exports.IRBlock = IRBlock;
// ==========================================
// 1. BLOKI AKCJI STRUMIENIA I EKSTRAKCJI
// ==========================================
class IRTokenAssign extends IRBlock {
    constructor(token_type) {
        super();
        this.token_var_id = -1;
        this.emit = (build_context, emitter) => {
            const var_name = `var_${this.token_var_id}`;
            emitter.emit_line(`auto ${var_name} = parser_context.see_current_token().as<CLua::${this.token_type}>();`);
            emitter.emit_line(`parser_context.get_next_token();`);
        };
        this.token_type = token_type;
    }
    process_step(build_context) {
        if (this.token_var_id === -1) {
            this.token_var_id = build_context.get_new_variable_id(this.constructor.name);
        }
        return false;
    }
}
exports.IRTokenAssign = IRTokenAssign;
class IRInvokeParserRule extends IRBlock {
    constructor(rule_name) {
        super();
        this.rule_name = rule_name;
        this.rule_var_id = -1;
        this.emit = (build_context, emitter) => {
            const var_name = `var_${this.rule_var_id}`;
            emitter.emit_line(`auto ${var_name} = Expression::expect_${this.rule_name}(parser_context);`);
        };
    }
    process_step(build_context) {
        if (this.rule_var_id === -1) {
            this.rule_var_id = build_context.get_new_variable_id(this.constructor.name);
        }
        return false;
    }
}
exports.IRInvokeParserRule = IRInvokeParserRule;
class IRFetchLexerValue extends IRBlock {
    constructor(value_kind) {
        super();
        this.value_kind = value_kind;
        this.value_var_id = -1;
        this.emit = (build_context, emitter) => {
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
    process_step(build_context) {
        if (this.value_var_id === -1) {
            this.value_var_id = build_context.get_new_variable_id(this.constructor.name);
        }
        return false;
    }
}
exports.IRFetchLexerValue = IRFetchLexerValue;
// ==========================================
// 2. BLOKI STRUKTURALNE I REFERENCJE
// ==========================================
class IRVarReference extends IRBlock {
    constructor(var_definition) {
        super();
        this.var_id = -1;
        this.emit = (build_context, emitter) => { };
        this.var_definition = var_definition;
        this.sync_id();
    }
    sync_id() {
        if (this.var_definition instanceof IRTokenAssign)
            this.var_id = this.var_definition.token_var_id;
        else if (this.var_definition instanceof IRInvokeParserRule)
            this.var_id = this.var_definition.rule_var_id;
        else if (this.var_definition instanceof IRFetchLexerValue)
            this.var_id = this.var_definition.value_var_id;
    }
    process_step(build_context) {
        this.sync_id();
        return false;
    }
}
exports.IRVarReference = IRVarReference;
class IRBindField extends IRBlock {
    constructor(target_field, source_var) {
        super();
        this.target_field = target_field;
        this.source_var = source_var;
        this.emit = (build_context, emitter) => {
            let source_id = -1;
            if (this.source_var instanceof IRTokenAssign)
                source_id = this.source_var.token_var_id;
            else if (this.source_var instanceof IRInvokeParserRule)
                source_id = this.source_var.rule_var_id;
            else if (this.source_var instanceof IRFetchLexerValue)
                source_id = this.source_var.value_var_id;
            emitter.emit_line(`// Mapping register var_${source_id} to node field: ${this.target_field.identifier}`);
        };
    }
    process_step(build_context) { return false; }
}
exports.IRBindField = IRBindField;
class IRCreateASTNode extends IRBlock {
    constructor(node_name, bound_fields) {
        super();
        this.node_name = node_name;
        this.bound_fields = bound_fields;
        this.emit = (build_context, emitter) => {
            const args = this.bound_fields.map(bf => {
                let source_id = -1;
                if (bf.source_var instanceof IRTokenAssign)
                    source_id = bf.source_var.token_var_id;
                else if (bf.source_var instanceof IRInvokeParserRule)
                    source_id = bf.source_var.rule_var_id;
                else if (bf.source_var instanceof IRFetchLexerValue)
                    source_id = bf.source_var.value_var_id;
                return `var_${source_id}`;
            }).join(", ");
            emitter.emit_line(`auto result_node = parser_context.create_node<${this.node_name}>(${args});`);
            emitter.emit_line(`return result_node;`);
        };
    }
    process_step(build_context) { return false; }
}
exports.IRCreateASTNode = IRCreateASTNode;
// ==========================================
// 3. LOGICZNE KONTENERY ZAKRESU (Scope Boundary)
// ==========================================
class IRScope extends IRBlock {
    constructor(scope_name, is_function = false) {
        super();
        this.scope_name = scope_name;
        this.is_function = is_function;
        this.ir_nodes = new Array();
        this.emit = (build_context, emitter) => {
            const execute_body = () => {
                this.ir_nodes.forEach(element => {
                    if (element instanceof IRBlock) {
                        element.emit(build_context, emitter);
                    }
                });
            };
            if (this.is_function) {
                emitter.emit_function(`NodeHandle expect_${this.scope_name}(ParserContext& parser_context)`, execute_body);
            }
            else {
                emitter.emit_namespace(`namespace ${this.scope_name}`, execute_body);
            }
        };
    }
    insert_ir(ir_block) {
        this.ir_nodes.push(ir_block);
    }
    process_step(build_context) {
        let changed = false;
        this.ir_nodes.forEach(node => {
            if (node instanceof IRBlock && node.process_step(build_context)) {
                changed = true;
            }
        });
        return changed;
    }
}
exports.IRScope = IRScope;
