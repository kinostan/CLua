import {Pattern as P} from "#common/pattern";

export class Field {
    identifier: string;
    field_pattern: P.PatternType;
    
    constructor(identifier: string,field_pattern: P.PatternType)
    {
        this.identifier = identifier;
        this.field_pattern = field_pattern;
    };
};

export class NodeDefinition {
    fields: Array<Field> = new Array<Field>();
    debug_name?: string;

    constructor(debug_name?: string)
    {
        if (!debug_name)
        {
            return;
        };
        this.debug_name = debug_name;
    };

    insert_field(field: Field): this
    {
        let has_field = this.fields.find((that_field) => {
            return that_field.identifier == field.identifier;
        });

        if (has_field)
        {
            throw new Error(
                `Field names can't repeat: ${field.identifier}`
            );
        };

        this.fields.push(field);
        return this;
    };
};