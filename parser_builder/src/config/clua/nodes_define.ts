// config/nodes_define.ts
import { NodeID } from "./nodes_declare";
import { NodeDefinition, Field } from "#root/common/ast/node";
import { Pattern as P } from "#common/pattern";
import * as LangDefinition from "./grammar"
import { as_node_chain, as_span } from "#root/common/patterns/conversions";

export const NodeRegistry = new Map<NodeID, NodeDefinition>();

NodeRegistry.set(
    NodeID.Identifier, 
    new NodeDefinition("IdentifierNode")
        .insert_field(new Field("span", P.as_span(LangDefinition.GenericIdentifier))) 
);

NodeRegistry.set(
    NodeID.LocalDeclaration,
    new NodeDefinition("LocalDeclNode")
        .insert_field(new Field("identifier", LangDefinition.GenericIdentifier))
);

NodeRegistry.set(
    NodeID.FunctionCall, 
    new NodeDefinition("FuncCallNode")
        .insert_field(new Field("func_name", as_span(LangDefinition.GenericIdentifier)))
        .insert_field(new Field("argument", as_node_chain(LangDefinition.ArgumentList)))
);

NodeRegistry.set(
    NodeID.StringLiteral,
    new NodeDefinition("StringPrimitive")
        .insert_field(
            new Field("span", as_span(LangDefinition.StringContent))
        )
)
    
NodeRegistry.set(
    NodeID.Root,
    new NodeDefinition("Root")
        .insert_field(new Field("next_node",as_node_chain(LangDefinition.SourceRoot)))
)