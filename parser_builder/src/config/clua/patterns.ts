import { NodeID } from "#config/clua/nodes_declare";
import * as P from "#common/pattern";
import { ErrorType } from "#root/common/parser_error";

export namespace Patterns {
    export const ExpressionPattern = new P.ChoicePattern();
 
    export const GroupExpressionPattern = new P.Pattern()
    .insert_pattern(new P.MatchSymbolToken("(").with_error(ErrorType.UnexpectedTokenError))
    .insert_pattern(ExpressionPattern) 
    .insert_pattern(new P.MatchSymbolToken(")").with_error(ErrorType.UnexpectedTokenError))
    .yields_node(NodeID.GroupExpressionNode)

    export const Root = new P.Pattern().insert_pattern(
        new P.QuantityPattern(
            new P.ChoicePattern().insert_pattern(ExpressionPattern)
        )
    );
};