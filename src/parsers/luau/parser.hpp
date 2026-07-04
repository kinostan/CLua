#pragma once

#include <common/language_processing/parser.hpp>
#include <common/language_processing/node_handle.hpp>

/* 
 Parser per language has a very specific set of 
*/

namespace LuaU {  
    enum class Keywords {
        Unknown
    };

    class LuaUParser: public Common::Parser::IParser {
        using NodeHandle = AST::NodeHandle;
        
        NodeHandle generate_AST(Common::Parser::ParserContext& parser_context) override {

        };
    };
}