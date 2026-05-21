export type FieldSize = 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128;


type CommonTypeNamespace = "Common";
type RawNumberType = "uint8" | "uint16" 
| "uint32" | "uint64" 
| "f32" | "f64";

type Value_NumberType = `${CommonTypeNamespace}::${RawNumberType}`; 
type Reference_NumberType = `${Value_NumberType}&`;

export type NumberType = Value_NumberType | Reference_NumberType;

type Value_ParserType = "ParserContext" | "NodeHandle" | "TokenGeneric" | "TokenSpan";
type Reference_ParserType = `${Value_ParserType}&`;

type ParserType = Value_ParserType | Reference_ParserType;

export type Type = NumberType | ParserType;

export const FieldTypeToSizeMap: Map<Type,FieldSize> = new Map<Type,FieldSize>();

FieldTypeToSizeMap.set("ParserContext",128);
FieldTypeToSizeMap.set("TokenSpan",32);
FieldTypeToSizeMap.set("TokenGeneric",16);
FieldTypeToSizeMap.set("NodeHandle",8);

FieldTypeToSizeMap.set("Common::f64",8);
FieldTypeToSizeMap.set("Common::f32",4);

FieldTypeToSizeMap.set("Common::uint64",8);
FieldTypeToSizeMap.set("Common::uint32",4);
FieldTypeToSizeMap.set("Common::uint16",2);
FieldTypeToSizeMap.set("Common::uint8",1);

export function set_field_type(field_type: Type, size: FieldSize) {
    if (FieldTypeToSizeMap.has(field_type))
    {
        throw new Error(`trying to set the size of the same field twice ${field_type}`);
    };
    FieldTypeToSizeMap.set(field_type,size);
};
