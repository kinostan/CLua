#include <parser/parser.hpp>

#include <windows.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

static std::string run_parser_print(const char* input)
{
    Util::Source source(
        reinterpret_cast<unsigned char*>(const_cast<char*>(input)),
        std::strlen(input)
    );

    ASTParser::Parser parser(source);

    std::ostringstream output_stream;
    auto* old_buffer = std::cout.rdbuf(output_stream.rdbuf());

    auto ast_root = parser.generate_AST();
    parser.print_node_tree(ast_root);

    std::cout.rdbuf(old_buffer);
    return output_stream.str();
}

int main()
{
    auto output = run_parser_print("((a::b::c::d.e.f.g.h.j))");

    assert(output.find("IdentifierPath: a") != std::string::npos);
    assert(output.find(":: b") != std::string::npos);
    assert(output.find(":: c") != std::string::npos);
    assert(output.find(":: d") != std::string::npos);
    assert(output.find(". e") != std::string::npos);
    assert(output.find(". f") != std::string::npos);
    assert(output.find(". g") != std::string::npos);
    assert(output.find(". h") != std::string::npos);
    assert(output.find(". j") != std::string::npos);

    std::cout << "Parser print test passed\n";
    std::cout << output;
}