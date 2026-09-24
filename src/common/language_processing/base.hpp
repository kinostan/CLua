#pragma once

#include <common/base.hpp>

namespace Common{
    struct TokenSpan {
        Common::uint64 start;
        Common::uint64 end;

        TokenSpan():start(0), end(0){};
        TokenSpan(Common::uint64 start,Common::uint64 end): start(start), end(end)
        {};
    };
}
