import {Pattern, PatternType} from "#common/pattern";

export class Field {
    identifier: string;
    field_pattern: PatternType;
    
    constructor(identifier: string,field_pattern: PatternType)
    {
        this.identifier = identifier;
        this.field_pattern = field_pattern;
    };
};

export class NodeDefinition {
    fields: Array<Field> = new Array<Field>();

    constructor()
    {
        
    };

    insert_field(field: Field)
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
    };
};