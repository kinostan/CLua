import {
    NumberType, Type, 
    FieldSize, FieldTypeToSizeMap
} from "./types";

class EnumClassDescription {
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

class ClassDescription {
    class_name: string = "";
    inherited_class?: string;

    fields: Map<string,Type> = new Map<string,Type>();

    get_sorted_fields() {
        const fields_array = Array.from(this.fields.entries()).map(([name, type]) => {
            return {
                name: name,
                type: type,
                size: FieldTypeToSizeMap.get(type) ?? (()=>{
                    throw new Error(`[Generator Error] Undefined field type: "${type}" on field "${name}"`)
                })()
            };
        });

        fields_array.sort((field_a, field_b) => field_b.size - field_a.size);

        return fields_array;
    }
};

export {
    EnumClassDescription,
    ClassDescription
};