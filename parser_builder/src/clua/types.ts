type FieldSize = 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128;

type CommonTypeNamespace = "Common";
type RawNumberType = "uint8" | "uint16" 
| "uint32" | "uint64" 
| "f32" | "f64";
type NumberType = `${CommonTypeNamespace}::${RawNumberType}`; 

type CLuaNamespace = "CLua";

type RawCLuaTypes = "NodeHandle" | "TokenGeneric" | "TokenSpan";
type CLuaTypes = `${CLuaNamespace}::${RawCLuaTypes}`;

type Type = NumberType | CLuaTypes;

const FieldTypeToSizeMap: Map<Type | string,number> = new Map<Type | string,number>();

FieldTypeToSizeMap.set("CLua::TokenSpan",32);
FieldTypeToSizeMap.set("CLua::TokenGeneric",16);
FieldTypeToSizeMap.set("CLua::NodeHandle",8);

FieldTypeToSizeMap.set("Common::f64",8);
FieldTypeToSizeMap.set("Common::f32",4);

FieldTypeToSizeMap.set("Common::uint64",8);
FieldTypeToSizeMap.set("Common::uint32",4);
FieldTypeToSizeMap.set("Common::uint16",2);
FieldTypeToSizeMap.set("Common::uint8",1);

function set_field_type(field_type: Type | string, size: FieldSize) {
    if (FieldTypeToSizeMap.has(field_type))
    {
        throw new Error(`trying to set the size of the same field twice ${field_type}`);
    };
    FieldTypeToSizeMap.set(field_type,size);
};

export {
    NumberType,
    Type,
    FieldSize,
    FieldTypeToSizeMap,

    set_field_type
}