#pragma once
#include "ast_nodes/base/base.hpp"

namespace CLuaNodes {
    class CharLiteral {
        char value;
    };

    class StringLiteral {
        char* string;
    };

    class NumberLiteral {
        long double value;
    };

    class IntegerLiteral {
        unsigned long long value;
    };
}
