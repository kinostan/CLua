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
    .yields_node(NodeID.Identifier) 
    .set_pattern_name("GenericIdentifier");

export const CommentStart = new P.MatchSymbolPattern("--", "COMMENT_START");
export const NewLineChar = new P.MatchSymbolPattern("\n", "NEWLINE");
    
export const CommentContent = new P.InvertedPattern()
    .insert_terminator(NewLineChar)
    .set_pattern_name("CommentContent");

export const LineComment = new P.Pattern()
    .insert_pattern(CommentStart)
    .insert_pattern(CommentContent)
    .insert_pattern(NewLineChar)
    .yields_node(NodeID.Comment)
    .set_pattern_name("LineComment");

export const QuoteChar = new P.MatchSymbolPattern('"', "QUOTE");

// InvertedPattern konsumuje całą zawartość stringa, dopóki nie napotka zamykającego cudzysłowu
export const StringContent = new P.Pattern()
    .insert_pattern(
        new P.InvertedPattern()
                /*
                    InvertedPattern should be inside of a pattern 
                    because that's how compiler distinguishes them
                */
            .insert_terminator(QuoteChar)   
            .set_pattern_name("StringContent")
    ).set_pattern_name("StringPrimitive"); 

export const StringLiteral = new P.Pattern()
    .insert_pattern(QuoteChar)
    .insert_pattern(StringContent)
    .insert_pattern(QuoteChar)
    .yields_node(NodeID.StringLiteral)
    .set_pattern_name("StringLiteral");

// Unikalny klucz kontekstu dla długości długiego komentarza
export const LongCommentLengthCtx = new P.LengthContext();

export const LongCommentOpenBracket = new P.MatchSymbolPattern("[", "OPEN_BRACKET");
export const LongCommentCloseBracket = new P.MatchSymbolPattern("]", "CLOSE_BRACKET");
export const EqualSign = new P.MatchSymbolPattern("=", "EQUAL_SIGN");

export const CaptureEquals = new P.CaptureLengthPattern(
    new P.QuantityPattern(EqualSign, 0, -1), // Dowolna liczba znaków '=' (od 0 wzwyż)
    LongCommentLengthCtx
);

export const MatchEquals = new P.MatchContextLengthPattern(
    new P.QuantityPattern(EqualSign, 0, -1),
    LongCommentLengthCtx
);

// Kompletny znacznik otwarcia: --[===[
export const LongCommentStart = new P.Pattern()
    .insert_pattern(new P.MatchSymbolPattern("--", "COMMENT_START"))
    .insert_pattern(LongCommentOpenBracket)
    .insert_pattern(CaptureEquals)
    .insert_pattern(LongCommentOpenBracket)
    .set_pattern_name("LongCommentStart");

// Kompletny znacznik zamknięcia: ]===]
export const LongCommentEnd = new P.Pattern()
    .insert_pattern(LongCommentCloseBracket)
    .insert_pattern(MatchEquals)
    .insert_pattern(LongCommentCloseBracket)
    .set_pattern_name("LongCommentEnd");

export const LongCommentContent = new P.InvertedPattern()
    .insert_terminator(LongCommentEnd)
    .set_inclusive(false)
    .set_pattern_name("LongCommentContent");

export const NumberLiteral = new P.Pattern()
    .insert_pattern(
        new P.QuantityPattern(
            Digit,1,10
        )
    )

export const Expression = new P.ChoicePattern()
    .insert_pattern(StringLiteral)
    .insert_pattern(NumberLiteral)
    .insert_pattern(GenericIdentifier)
    .set_pattern_name("Expression");

export const LocalKeyword = new P.MatchSymbolPattern("local", "KEYWORD");

export const LocalDeclaration = new P.Pattern()
    .insert_pattern(LocalKeyword)
    .insert_pattern(RequiredWhitespace)
    .insert_pattern(GenericIdentifier)
    .yields_node(NodeID.LocalDeclaration)
    .set_pattern_name("LocalDeclaration");

export const FunctionCall = new P.Pattern()
    .insert_pattern(GenericIdentifier)
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(new P.MatchSymbolPattern("(", "OPEN_PAREN"))
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(GenericIdentifier)
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(new P.MatchSymbolPattern(")", "CLOSE_PAREN"))
    .yields_node(NodeID.FunctionCall)
    .set_pattern_name("FunctionCall");

export const StatementParser = new P.ChoicePattern()
    .insert_pattern(LocalDeclaration)
    .insert_pattern(FunctionCall)
    .set_pattern_name("StatementParser");

export const Trivia = new P.ChoicePattern()
    .insert_pattern(OptionalWhitespace)
    .insert_pattern(LineComment);

export const StatementWithTrivia = new P.Pattern()
    .insert_pattern(Trivia)
    .insert_pattern(StatementParser);

// Główny węzeł programu - reprezentuje cały plik źródłowy
export const SourceRoot = new P.QuantityPattern(StatementWithTrivia, 0, -1)
    .yields_node(NodeID.AST_ROOT)
    .set_pattern_name("SourceRoot");