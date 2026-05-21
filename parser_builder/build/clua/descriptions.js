"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.NamespaceDescription = exports.ClassDescription = exports.EnumClassDescription = void 0;
const types_1 = require("./types");
class EnumClassDescription {
    constructor(enum_name) {
        this.enum_name = "";
        this.enum_list = [];
        this.enum_name = enum_name;
    }
    ;
    add_list_element(field) {
        if (this.enum_list.find((value) => field === value)) {
            throw new Error(`
                Names of fields in an enum can't repeat: ${this.enum_name} found a repetition of a field ${field}
            `);
        }
        ;
        this.enum_list.push(field);
    }
    ;
    get_enum_size() {
        if (this.enum_list.length > 256) {
            return 2;
        }
        ;
        return 1;
    }
    ;
    get_enum_size_unit() {
        if (this.get_enum_size() == 2) {
            return "Common::uint16";
        }
        ;
        return "Common::uint8";
    }
    ;
}
exports.EnumClassDescription = EnumClassDescription;
;
class ClassDescription {
    constructor(class_name, inherited_class, fields_input = []) {
        this.class_name = "";
        this.fields = new Map();
        this.class_name = class_name;
        if (inherited_class) {
            this.inherited_class = inherited_class;
        }
        // Populate the Map with the provided field definitions
        for (const [field_name, field_type] of fields_input) {
            this.fields.set(field_name, field_type);
        }
    }
    get_sorted_fields() {
        const fields_array = Array.from(this.fields.entries()).map(([name, type]) => {
            return {
                name: name,
                type: type,
                size: types_1.FieldTypeToSizeMap.get(type) ?? (() => {
                    throw new Error(`[Generator Error] Undefined field type: "${type}" on field "${name}"`);
                })()
            };
        });
        fields_array.sort((field_a, field_b) => field_b.size - field_a.size);
        return fields_array;
    }
    ;
}
exports.ClassDescription = ClassDescription;
;
class NamespaceDescription {
    constructor(name) {
        this.name = "";
        if (!name) {
            throw new Error("namespace must be not empty in name");
        }
        ;
        this.name = name;
    }
    ;
}
exports.NamespaceDescription = NamespaceDescription;
;
