#pragma once

#include <common/base.hpp>

namespace Common{
    struct TokenSpan {
        Common::uint64 start;
        Common::uint64 end;

        TokenSpan(Common::uint64 start,Common::uint64 end): start(start), end(end)
        {};
    };
}
