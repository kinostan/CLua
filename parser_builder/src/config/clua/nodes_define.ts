// config/nodes_define.ts
import { NodeID } from "./nodes_declare";
import { NodeDefinition, Field } from "#root/common/node";
import * as P from "#common/pattern";
import * as LangDefinition from "./patterns"

export const NodeRegistry = new Map<NodeID, NodeDefinition>();

// 1. Węzeł reprezentujący surowy identyfikator w AST
NodeRegistry.set(
    NodeID.AST_IDENTIFIER, // Odpowiednik yields_node(101) z gramatyki
    new NodeDefinition("IdentifierNode")
        // TokenSpan przechowuje offsety start i end bezpośrednio z bufora pliku źródłowego
        .insert_field(new Field("span", P.as_span(LangDefinition.GenericIdentifier))) 
);

// 2. Węzeł deklaracji lokalnej: local <identifier>
NodeRegistry.set(
    NodeID.AST_LOCAL_DECL, // Odpowiednik yields_node(201) z gramatyki
    new NodeDefinition("LocalDeclNode")
        // Wskazuje na węzeł typu IdentifierNode zaimplementowany w arenie
        .insert_field(new Field("identifier", LangDefinition.GenericIdentifier))
);

// 3. Węzeł wywołania funkcji: <identifier>( <identifier> )
NodeRegistry.set(
    NodeID.AST_FUNC_CALL, // Odpowiednik yields_node(202) z gramatyki
    new NodeDefinition("FuncCallNode")
        // Nazwa funkcji (IdentifierNode)
        .insert_field(new Field("func_name", LangDefinition.GenericIdentifier))
        // Przekazywany argument (IdentifierNode)
        .insert_field(new Field("argument", LangDefinition.GenericIdentifier))
);