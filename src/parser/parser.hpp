#pragma once

#include <common/language_processing/parser.hpp>
#include <common/language_processing/node_handle.hpp>

#include "nodes.hpp"
#include "symbols.hpp"

/* 
 Parser per language has a very specific set of 
*/

namespace CLua {  
    class Parser: public Common::IParser {
       AST::NodeHandle generate_AST(Common::ParserContext& parser_context) override;
    };
}