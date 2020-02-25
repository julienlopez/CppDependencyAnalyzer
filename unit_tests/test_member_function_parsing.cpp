#include <catch2/catch.hpp>

#include "classparser.hpp"

#include "classgenerator.hpp"

using namespace Testing;

namespace
{

auto parseClassWithFunction(std::vector<std::wstring> lines)
{
    ClassGenerator cg;
    for(auto& line : lines)
        cg.addLine(std::move(line));
    return Cda::ClassParser().parseClass(cg);
}

} // namespace

TEST_CASE("Function parsing of ClassParser", "[ClassParser]")
{
    SECTION("Class parsing with a simple virtual function")
    {
        const auto res = parseClassWithFunction({L"virtual int run(int i);"});
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().is_virtual);
    }

    SECTION("Class parsing with a simple virtual function marked override")
    {
        const auto res = parseClassWithFunction({L"virtual int run(int i) override;"});
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().is_virtual);
    }

    SECTION("Class parsing with a simple function marked override")
    {
        const auto res = parseClassWithFunction({L"int run(int i) override;"});
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().is_virtual);
    }

    SECTION("Class parsing with a simple const function marked override")
    {
        const auto res = parseClassWithFunction({L"int run(int i) const override;"});
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().is_virtual);
    }

    SECTION("Bug in class parsing with inlines functions")
    {
        // test both that the return is not null, and that it doesn't throw
        CHECK_NOTHROW([]() { REQUIRE(parseClassWithFunction({L" A()", L"{", L"}"})); });
    }

    SECTION("Class parsing of a function returning a complex templated type")
    {
        const auto res = parseClassWithFunction({L"std::pair<int, std::string> run(int i) const;"});
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
    }

    SECTION("parsing a function returning a const type")
    {
        const auto res = parseClassWithFunction({L"const std::string& run(int i) const;"});
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
    }
}
