// config/nodes_define.ts
import { NodeID } from "./nodes_declare";
import { NodeDefinition, Field } from "#root/common/ast/node";
import { Pattern as P } from "#common/pattern";
import * as LangDefinition from "./grammar"
import { as_span } from "#root/common/patterns/conversions";

export const NodeRegistry = new Map<NodeID, NodeDefinition>();

NodeRegistry.set(
    NodeID.AST_IDENTIFIER, 
    new NodeDefinition("IdentifierNode")
        .insert_field(new Field("span", P.as_span(LangDefinition.GenericIdentifier))) 
);

NodeRegistry.set(
    NodeID.AST_LOCAL_DECL,
    new NodeDefinition("LocalDeclNode")
        .insert_field(new Field("identifier", LangDefinition.GenericIdentifier))
);

NodeRegistry.set(
    NodeID.AST_FUNC_CALL, 
    new NodeDefinition("FuncCallNode")
        .insert_field(new Field("func_name", LangDefinition.GenericIdentifier))
        .insert_field(new Field("argument", LangDefinition.GenericIdentifier))
);

NodeRegistry.set(
    NodeID.AST_STRING_LITERAL,
    new NodeDefinition("StringPrimitive")
        .insert_field(
            new Field("span", as_span(LangDefinition.StringContent))
        )
)
    