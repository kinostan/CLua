#pragma once

#include <common/language_processing/parser.hpp>
#include <common/language_processing/node_handle.hpp>

#include "nodes.hpp"
#include "keywords.hpp"

/* 
 Parser per language has a very specific set of 
*/

namespace CLua {  
    class CLuaParser: public Common::Parser::IParser {
       AST::NodeHandle generate_AST(Common::Parser::ParserContext& parser_context) override;
    };
}