import * as Types from "#root/common/parser-types";
import {
    Field
} from "#common/descriptions";

class BaseNode {
    field_list: Array<Field> = new Array<Field>();

    insert_field(type: Types.Type,field_name: string,default_expression?: string): this
    {
        this.field_list.push(
            new Field(type,field_name,default_expression = "")
        );
        return this;
    };

    constructor()
    {
        this.insert_field("NodeType","node_type","NodeType::None");
    };
};

class ASTNode extends BaseNode {};

let ast_nodes: Array<ASTNode> = new Array<ASTNode>();

function insert_ast_node(ast_node: ASTNode)
{
    let uid = ast_nodes.length;
    ast_nodes.push(ast_node);
    return uid;
};

let number_literal_node = new ASTNode().insert_field(
    "Common::f64", "fraction", "0.0"
).insert_field(
    "Common::uint64", "integer", "0"
);

let string_literal_node = new ASTNode().insert_field(
    "TokenGeneric","string_token","TokenGeneric()"
);

let char_literal_node = new ASTNode().insert_field(
    "char",
    "char_value",
    "0"
);

export namespace CLuaNodes {
    export const NumberLiteralNode = insert_ast_node(number_literal_node);
    export const StringLiteralNode = insert_ast_node(string_literal_node);
    export const CharLiteralNode = insert_ast_node(char_literal_node);
};

export function get_node_schema_by_id(node_id: number)
{
    return ast_nodes[node_id];
};
