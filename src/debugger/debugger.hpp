#pragma once

#include <iostream>
#include <string>
   
#ifdef _DEBUG
namespace DebugInfo {
    using namespace std::literals::string_literals;

    constexpr auto ParserError = "Parser Error: "s;
    constexpr auto ParserErrorEnd = "\n"s;
};

#define Assert(statement,message)\
    LexerDebug::assert(statement, __FILE__ , __LINE__, DebugInfo::ParserError + message + DebugInfo::ParserErrorEnd);

#define AssertEq(val1,val2)\
    LexerDebug::assert(val1 == val2,__FILE__ , __LINE__, #val1 " is not equal to " #val2);

#define AssertNull(val)\
    LexerDebug::assert(val != nullptr,__FILE__ , __LINE__, "value is a nullptr");
#else
    #define Assert(statement,message) ((void)0);
    #define AssertEq(val1,val2) ((void)0);
    #define AssertNull(val) ((void)0);
#endif


namespace LexerDebug {
    inline void assert(bool statement_value,const char* File,int Line,std::string Message)
    {
        if (statement_value){
            return;
        };
        std::cerr << File << "::" << Line << " Error Message:\n" << Message;
        exit(1);
    };
};