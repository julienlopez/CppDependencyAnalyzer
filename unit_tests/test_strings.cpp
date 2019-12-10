#include <catch2/catch.hpp>

#include "utils/strings.hpp"

TEST_CASE("Basic uses of Utils::Strings", "[utils]")
{
    using namespace Cda::Utils::Strings;

    using namespace std::string_literals;

    using Catch::Matchers::Equals;

    SECTION("startsWith")
    {
        CHECK(startsWith(L"ab"s, L"a"s));
        CHECK(startsWith(L"ab"s, L"b"s) == false);
        CHECK(startsWith(L" ab"s, L"a"s) == false);
    }

    SECTION("trim")
    {
        CHECK(trim(L"ab"s) == L"ab"s);
        CHECK(trim(L" ab"s) == L"ab"s);
        CHECK(trim(L"ab "s) == L"ab"s);
        CHECK(trim(L" ab "s) == L"ab"s);
        CHECK(trim(L" \t ab "s) == L"ab"s);
    }
}
