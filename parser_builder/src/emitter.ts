import { EnumClassDescription, ClassDescription } from "./clua/descriptions";
import { set_field_type } from "./clua/types"

class BaseEmitter {
    code: string = "";
    indent: number = 0;
    indent_text: string = "";

    constructor()
    {
        this.code = "";
    };

    emit(emit_code: string)
    {
        this.code += this.indent_text + emit_code + '\n';
    };

    step_indent()
    {
        this.indent++;
        this.indent_text = " ".repeat(this.indent);
    };

    step_dedent()
    {
        this.indent--;
        if (this.indent < 0)
        {
            throw new Error("indent is less than 0, which is illegal I guess...");
        };
        this.indent_text = " ".repeat(this.indent);
    };
};

class CLuaEmitter extends BaseEmitter {
    constructor()
    {
        super();
    };

    emit_class(class_object: ClassDescription) {

        let inheritance = class_object.inherited_class ? `: public ${class_object.inherited_class}` : ``;

        this.emit(`class ${class_object.class_name}${inheritance}{`);
        this.step_indent();
        this.emit(`public:`);

        let field_list = class_object.get_sorted_fields();
        for (let index = 0; index < field_list.length; index++) {
            const field_element = field_list[index];
            
            this.emit(`${field_element.type} ${field_element.name} = ${field_element.type}{0};`);
        };

        this.step_dedent();
        this.emit(`};`);
    };

    emit_enum(enum_class_object: EnumClassDescription)
    {
        this.emit(`enum class ${enum_class_object.enum_name} {`);
        this.step_indent();

        let size_of = enum_class_object.get_enum_size();
        set_field_type(enum_class_object.enum_name,size_of);//ig setter ignores namespaces but it can be fixed in the future

        for (let index = 0; index < enum_class_object.enum_list.length; index++) {
            const element_name = enum_class_object.enum_list[index];
            const comma_postfix = (index+1 < enum_class_object.enum_list.length) ?  `,` : ``;
            this.emit(`${element_name}${comma_postfix}`);
        }

        this.step_dedent();
        this.emit(`};`);
    };
};

class LuaUEmitter extends BaseEmitter {

};