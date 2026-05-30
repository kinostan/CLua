// config/patterns.ts
import { NodeID } from "./nodes_declare";
import * as P from "#common/pattern";

export namespace Patterns {
    export const IdentifierPattern = new P.MatchIdentifierToken();
    export const ExpressionPattern = new P.ChoicePattern();
 
    export const LocalAssignPattern = new P.Pattern()
        .insert_pattern(new P.MatchKeywordToken())   
        .insert_pattern(new P.MatchIdentifierToken()) 
        .insert_pattern(new P.MatchSymbolToken("="))  
        .yields_node(NodeID.LocalAssignNode);

    export const Root = new P.QuantityPattern(
        new P.ChoicePattern()
            .insert_pattern(IdentifierPattern)
            .insert_pattern(ExpressionPattern)
            .insert_pattern(LocalAssignPattern),
        0,
        -1
    );
};