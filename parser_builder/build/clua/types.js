"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.FieldTypeToSizeMap = void 0;
exports.set_field_type = set_field_type;
exports.FieldTypeToSizeMap = new Map();
exports.FieldTypeToSizeMap.set("CLua::TokenSpan", 32);
exports.FieldTypeToSizeMap.set("CLua::TokenGeneric", 16);
exports.FieldTypeToSizeMap.set("CLua::NodeHandle", 8);
exports.FieldTypeToSizeMap.set("Common::f64", 8);
exports.FieldTypeToSizeMap.set("Common::f32", 4);
exports.FieldTypeToSizeMap.set("Common::uint64", 8);
exports.FieldTypeToSizeMap.set("Common::uint32", 4);
exports.FieldTypeToSizeMap.set("Common::uint16", 2);
exports.FieldTypeToSizeMap.set("Common::uint8", 1);
function set_field_type(field_type, size) {
    if (exports.FieldTypeToSizeMap.has(field_type)) {
        throw new Error(`trying to set the size of the same field twice ${field_type}`);
    }
    ;
    exports.FieldTypeToSizeMap.set(field_type, size);
}
;
