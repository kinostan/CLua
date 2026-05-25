import * as BasicPatterns from "../common/pattern";
import * as Errors from "../config/errors";
import {
    CLuaNodes
} from "#config/nodes";
import { ValidKeyword } from "#root/common/clua_types";
import { BaseEmitter } from "#root/common/emitter";

export let clua_pattern_map: Map<string, BasicPatterns.PatternType> = new Map();

function set_pattern<T extends BasicPatterns.Pattern>(name: string, pattern: T): T {
    clua_pattern_map.set(name, pattern);
    return pattern;
}

let Statement = new BasicPatterns.Pattern(
    0,"statement"
);

let Expression = new BasicPatterns.Pattern(
    0,"expression"
).insert_pattern(
    new BasicPatterns.ActionPattern("set expression node",(emitter: BaseEmitter, bind_context: BasicPatterns.BindContext) => {
        emitter.emit(`NodeHandle node = parser_context.create_node<${typeof CLuaNodes.ExpressionStatementNode}>()`);
})).set_node_template_id(CLuaNodes.ExpressionStatementNode);

let Declaration = new BasicPatterns.Pattern(
    0,"declaration"
).insert_pattern(
    new BasicPatterns.KeywordPattern<ValidKeyword>(0,"let")
).insert_pattern(
    new BasicPatterns.IdentifierPattern(0)
).insert_pattern(Expression);

let LineOfCode = new BasicPatterns.Pattern(
    0, "LOC"
).insert_pattern(
    (new BasicPatterns.OrPattern(0,"declaration expression statement"))
    .add_pattern(Statement)
    .add_pattern(Declaration)
    .add_pattern(Expression)
);

let Root = (new BasicPatterns.Pattern(BasicPatterns.NO_ERROR,"root")).insert_pattern(
    new BasicPatterns.QuantityPattern(LineOfCode,-1,-1)
);

let NumberPattern = set_pattern("number", 
    new BasicPatterns.NumberPattern(Errors.NumberPatternMessageId)
).set_node_template_id(CLuaNodes.NumberLiteralNode)

let StringPattern = set_pattern("string", new BasicPatterns.StringPattern(
    Errors.StringPatternMessageId)
).set_node_template_id(CLuaNodes.StringLiteralNode);

let CharPattern = set_pattern("char", 
    new BasicPatterns.CharPattern(Errors.CharPatternMessageId)
).set_node_template_id(CLuaNodes.CharLiteralNode);


let RootPattern = set_pattern("root", Root);

export {
    RootPattern
};