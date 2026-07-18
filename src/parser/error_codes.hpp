namespace CLua {
    enum class ErrorCode {
        Unspecified,
        ExpectedKeyword,
        ExpectedIdentifier,
        UnknownStatement,
        ExpectedCloseParen,
        ExpectedOpenParen,
        ExpectedWhitespace,
        ExpectedCommentStart,
        ExpectedCommentEnd,
        ExpectedNewline,
        ExpectedStringClosure
    };
};