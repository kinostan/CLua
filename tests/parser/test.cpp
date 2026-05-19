#include <parser/parser.hpp>

#include <windows.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

static void print_source(const char* input)
{
    Common::Source source(
        reinterpret_cast<unsigned char*>(const_cast<char*>(input)),
        std::strlen(input)
    );

    ASTParser::Parser parser(source);

    auto ast_root = parser.generate_AST();
    parser.print_node_tree(ast_root);
}

int main()
{
    print_source("(a::b::c::d::e::f)");

    // 2. Zagnieżdżone nawiasy (sprawdź, czy GroupExpression poprawnie się rekuruje)
    print_source("((42))");
    print_source("(((x)))");

    // 3. Puste napisy i znaki specjalne w stringach
    print_source("\"\""); 
    print_source("\"napis ze spacja i !@#\"");

    // 4. Test białych znaków (czy parser je ignoruje wewnątrz wyrażeń grupowych)
    print_source("(   123   )");
    print_source("(\n  b\n)");

    // 5. Wartości brzegowe dla liczb (pamiętając o poprawionej unii)
    print_source("0");
    print_source("0.00001");
    print_source("9007199254740991"); // Max bezpieczny f64 (2^53 - 1)

    // 6. Identyfikatory z podkreśleniami lub cyframi (jeśli wspierasz)
    print_source("variable_name");
    print_source("var123");

    print_source("-(var1)");
}