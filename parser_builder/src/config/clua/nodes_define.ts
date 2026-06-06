// config/nodes_define.ts
import { NodeID } from "./nodes_declare";
import { Patterns } from "./patterns";
import { NodeDefinition, Field } from "#root/common/node";

import * as P from "#common/pattern"

export const NodeRegistry = new Map<NodeID, NodeDefinition>();

NodeRegistry.set(
    NodeID.LocalAssignNode,
    new NodeDefinition()
        .insert_field(new Field("identifier", new P.MatchIdentifierToken()))
        .insert_field(new Field("value", Patterns.ExpressionPattern)) 
        .insert_field(new Field("next_node",new P.QuantityPattern()))
        //checks class_name when matching so technically speaking Patterns.MultitpleChoicePattern[X] 
        // can be linked but must have defined unique class (or rather should, it's not a must)
);