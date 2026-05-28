"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.NodeDefinition = exports.Field = void 0;
class Field {
    constructor(identifier, field_pattern) {
        this.identifier = identifier;
        this.field_pattern = field_pattern;
    }
    ;
}
exports.Field = Field;
;
class NodeDefinition {
    constructor() {
        this.fields = new Array();
    }
    ;
    insert_field(field) {
        let has_field = this.fields.find((that_field) => {
            return that_field.identifier == field.identifier;
        });
        if (has_field) {
            throw new Error(`Field names can't repeat: ${field.identifier}`);
        }
        ;
        this.fields.push(field);
    }
    ;
}
exports.NodeDefinition = NodeDefinition;
;
