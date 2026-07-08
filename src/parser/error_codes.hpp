namespace CLua {
    enum class ErrorCode {
        None,
        ExpectedKeyword,
        ExpectedIdentifier,
        UnknownStatement,
        ExpectedCloseParen,
        ExpectedOpenParen,
        ExpectedWhitespace,
        ExpectedCommentStart,
        ExpectedNewline,
    };
};