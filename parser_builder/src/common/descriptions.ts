import {
    NumberType, Type, 
    FieldSize, FieldTypeToSizeMap
} from "#common/parser-types";

export class Field {
    type: Type = "Common::uint8";
    field_name: string = "";
    default_expression: string = "";

    constructor(type: Type,field_name: string,default_expression: string)
    {
        this.type = type;
        this.field_name = field_name;
        this.default_expression = default_expression;
    };
};

export class EnumClassDescription {
    enum_name: string = "";
    enum_list: Array<string> = [];

    constructor(enum_name: string)
    {
        this.enum_name = enum_name;
    };

    add_list_element(field: string)
    {
        if (this.enum_list.find((value: string) => field === value)) {
            throw new Error(`
                Names of fields in an enum can't repeat: ${this.enum_name} found a repetition of a field ${field}
            `);
        };
        this.enum_list.push(field);
    };

    get_enum_size(): FieldSize
    {
        if (this.enum_list.length > 256)
        {
            return 2;
        };
        return 1;
    };

    get_enum_size_unit(): NumberType {
        if (this.get_enum_size() == 2)
        {
            return "Common::uint16";
        };
        return "Common::uint8";
    };
};

export class ClassDescription {
    class_name: string = "";
    inherited_class?: string;

    fields: Map<string,Field> = new Map<string,Field>();

    constructor(class_name: string, inherited_class: string | null) {
        this.class_name = class_name;

        if (inherited_class)
        {
            this.inherited_class = inherited_class;
        }
    }

    insert_field(field: Field)
    {
        this.fields.set(
            field.field_name,
            field
        );
    };

    get_sorted_fields() {
        const fields_array = Array.from(this.fields.entries()).map(([name, field]) => {
            return {
                field: field,
                size: FieldTypeToSizeMap.get(field.type) ?? (()=>{
                    throw new Error(`[Generator Error] Undefined field type: "${field.type}" on field "${name}"`)
                })()
            };
        });

        fields_array.sort((field_a, field_b) => field_b.size - field_a.size);

        return fields_array;
    };
};

export class NamespaceDescription{
    name: string = "";
    constructor(name: string)
    {
        if (!name)
        {
            throw new Error("namespace must be not empty in name");
        };

        this.name = name;
    };
};
