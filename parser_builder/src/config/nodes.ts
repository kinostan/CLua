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
    "char","char_value","0"
);

let boolean_literal_node = new ASTNode().insert_field(
    "Common::uint8","value","0"
);

let null_literal_node = new ASTNode();

let identifier_node = new ASTNode().insert_field(
    "TokenGeneric","name_token","TokenGeneric()"
);

let program_node = new ASTNode().insert_field(
    "NodeHandle","first_child","NodeHandle()"
).insert_field(
    "Common::uint64","child_count","0"
);

let variable_declaration_node = new ASTNode().insert_field(
    "NodeHandle","identifier","NodeHandle()"
).insert_field(
    "NodeHandle","type_annotation","NodeHandle()"
).insert_field(
    "NodeHandle","initializer","NodeHandle()"
);

let function_declaration_node = new ASTNode().insert_field(
    "TokenGeneric","name_token","TokenGeneric()"
).insert_field(
    "NodeHandle","parameters","NodeHandle()"
).insert_field(
    "Common::uint64","parameter_count","0"
).insert_field(
    "NodeHandle","body","NodeHandle()"
);

let parameter_node = new ASTNode().insert_field(
    "TokenGeneric","name_token","TokenGeneric()"
).insert_field(
    "NodeHandle","type_annotation","NodeHandle()"
).insert_field(
    "NodeHandle","default_value","NodeHandle()"
);

let block_node = new ASTNode().insert_field(
    "NodeHandle","first_statement","NodeHandle()"
).insert_field(
    "Common::uint64","statement_count","0"
);

let expression_statement_node = new ASTNode().insert_field(
    "NodeHandle","expression","NodeHandle()"
);

let return_statement_node = new ASTNode().insert_field(
    "NodeHandle","expression","NodeHandle()"
);

let if_statement_node = new ASTNode().insert_field(
    "NodeHandle","condition","NodeHandle()"
).insert_field(
    "NodeHandle","then_branch","NodeHandle()"
).insert_field(
    "NodeHandle","else_branch","NodeHandle()"
);

let for_statement_node = new ASTNode().insert_field(
    "NodeHandle","init","NodeHandle()"
).insert_field(
    "NodeHandle","condition","NodeHandle()"
).insert_field(
    "NodeHandle","increment","NodeHandle()"
).insert_field(
    "NodeHandle","body","NodeHandle()"
);

let while_statement_node = new ASTNode().insert_field(
    "NodeHandle","condition","NodeHandle()"
).insert_field(
    "NodeHandle","body","NodeHandle()"
);

let break_node = new ASTNode();

let continue_node = new ASTNode();

let binary_expression_node = new ASTNode().insert_field(
    "NodeHandle","left","NodeHandle()"
).insert_field(
    "TokenGeneric","operator_token","TokenGeneric()"
).insert_field(
    "NodeHandle","right","NodeHandle()"
);

let unary_expression_node = new ASTNode().insert_field(
    "TokenGeneric","operator_token","TokenGeneric()"
).insert_field(
    "NodeHandle","operand","NodeHandle()"
);

let call_expression_node = new ASTNode().insert_field(
    "NodeHandle","callee","NodeHandle()"
).insert_field(
    "NodeHandle","argument_list","NodeHandle()"
).insert_field(
    "Common::uint64","argument_count","0"
);

let member_expression_node = new ASTNode().insert_field(
    "NodeHandle","object","NodeHandle()"
).insert_field(
    "TokenGeneric","property_token","TokenGeneric()"
).insert_field(
    "Common::uint8","computed","0"
);

let assignment_node = new ASTNode().insert_field(
    "NodeHandle","left","NodeHandle()"
).insert_field(
    "NodeHandle","right","NodeHandle()"
);

export namespace CLuaNodes {
    export const NumberLiteralNode = insert_ast_node(number_literal_node);
    export const StringLiteralNode = insert_ast_node(string_literal_node);
    export const CharLiteralNode = insert_ast_node(char_literal_node);
    export const BooleanLiteralNode = insert_ast_node(boolean_literal_node);
    export const NullLiteralNode = insert_ast_node(null_literal_node);
    export const IdentifierNode = insert_ast_node(identifier_node);
    export const ProgramNode = insert_ast_node(program_node);
    export const VariableDeclarationNode = insert_ast_node(variable_declaration_node);
    export const FunctionDeclarationNode = insert_ast_node(function_declaration_node);
    export const ParameterNode = insert_ast_node(parameter_node);
    export const BlockNode = insert_ast_node(block_node);
    export const ExpressionStatementNode = insert_ast_node(expression_statement_node);
    export const ReturnStatementNode = insert_ast_node(return_statement_node);
    export const IfStatementNode = insert_ast_node(if_statement_node);
    export const ForStatementNode = insert_ast_node(for_statement_node);
    export const WhileStatementNode = insert_ast_node(while_statement_node);
    export const BreakNode = insert_ast_node(break_node);
    export const ContinueNode = insert_ast_node(continue_node);
    export const BinaryExpressionNode = insert_ast_node(binary_expression_node);
    export const UnaryExpressionNode = insert_ast_node(unary_expression_node);
    export const CallExpressionNode = insert_ast_node(call_expression_node);
    export const MemberExpressionNode = insert_ast_node(member_expression_node);
    export const AssignmentNode = insert_ast_node(assignment_node);
};

export function get_node_schema_by_id(node_id: number)
{
    return ast_nodes[node_id];
};
