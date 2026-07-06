import { ErrorTypes } from "./errors";
import { NodeID } from "./nodes_declare";
import { 
    Pattern as P
} from "#common/pattern"; 

export const SpaceChar = new P.CharRange().insert_range(" ", " ");
export const OptionalWhitespace = new P.QuantityPattern(SpaceChar, 0, -1);
export const RequiredWhitespace = new P.QuantityPattern(SpaceChar, 1, -1);

export const AlphaUnder = new P.CharRange()
    .insert_range("a", "z")
    .insert_range("A", "Z")
    .insert_range("_", "_");

export const Digit = new P.CharRange().insert_range("0", "9");

export const AlnumUnder = new P.CharRange()
    .insert_range_def(AlphaUnder)
    .insert_range_def(Digit)

export const IdentifierRest = new P.QuantityPattern(AlnumUnder, 0, -1);

export const GenericIdentifier = new P.Pattern()
    .insert_pattern(AlphaUnder)
    .insert_pattern(IdentifierRest)
    .yields_node(NodeID.AST_IDENTIFIER) 
    .set_pattern_name("GenericIdentifier");

export const LocalKeyword = new P.MatchSymbolPattern("local", "KEYWORD");


/*
    When locally pattern is ambigious 
    I should disambigiouate by looking at a higher level scope.
    Example:

    LocalKeyword + RequiredWhitespace is ambigious with 
    GenericIdentifier + OptionalWhitespace

    How to disambiguate: 

    Since GenericIdentifier doesn't start with "(" and it's the next required 
    symbol for FunctionCall that's where these 2 patterns disambiguate and that's
    how I know "local" is a true keyword
*/
export const LocalDeclaration = new P.Pattern()
    .insert_pattern(LocalKeyword)
    .insert_pattern(RequiredWhitespace)
    .insert_pattern(GenericIdentifier)
    .yields_node(NodeID.AST_LOCAL_DECL) // ID: AST_LOCAL_DECL
    .set_pattern_name("LocalDeclaration");

export const FunctionCall = new P.Pattern()
    .insert_pattern(GenericIdentifier)
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(new P.MatchSymbolPattern("(", "OPEN_PAREN"))
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(GenericIdentifier)
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(new P.MatchSymbolPattern(")", "CLOSE_PAREN"))
    .yields_node(NodeID.AST_FUNC_CALL) // ID: AST_FUNC_CALL
    .set_pattern_name("FunctionCall");

export const StatementParser = new P.ChoicePattern()
    .insert_pattern(LocalDeclaration)
    .insert_pattern(FunctionCall)
    .set_pattern_name("StatementParser");

