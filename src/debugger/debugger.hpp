#include <iostream>
#include <string>
   
#ifdef _DEBUG
namespace DebugInfo {
    using namespace std::literals::string_literals;

    constexpr auto ParserError = "Parser Error: "s;
    constexpr auto ParserErrorEnd = "\n"s;

    constexpr auto LexerError = "Lexer Error: "s;
    constexpr auto LexerErrorEnd = "\n"s;
};

#define Assert(statement,message)\
    LexerDebug::lexer_assert(statement, __FILE__ , __LINE__, message );

#define PAssert(statement,message)\
    LexerDebug::lexer_assert(statement, __FILE__ , __LINE__, DebugInfo::ParserError + message + DebugInfo::ParserErrorEnd);

#define LAssert(statement,message)\
    LexerDebug::lexer_assert(statement, __FILE__ , __LINE__, DebugInfo::LexerError + message + DebugInfo::LexerErrorEnd);

#define AssertEq(val1,val2)\
    LexerDebug::lexer_assert(val1 == val2,__FILE__ , __LINE__, #val1 " is not equal to " #val2);

#define AssertNull(val)\
    LexerDebug::lexer_assert(val != nullptr,__FILE__ , __LINE__, "value is a nullptr");
#else
    #define Assert(statement,message) ((void)0);
    #define LAssert(statement,message) ((void)0);
    #define PAssert(statement,message) ((void)0);
    #define AssertEq(val1,val2) ((void)0);
    #define AssertNull(val) ((void)0);
#endif


namespace LexerDebug {
    inline void lexer_assert(bool statement_value,const char* File,int Line,std::string Message)
    {
        if (statement_value){
            return;
        };
        std::cerr << File << "::" << Line << " Error Message:\n" << Message;
        exit(1);
    };
};