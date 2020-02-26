#include <catch2/catch.hpp>

#include "classparser.hpp"

#include "classgenerator.hpp"

using namespace Testing;
using namespace std::string_literals;

TEST_CASE("Parsing of classes inside namespaces", "[ClassParser]")
{

    SECTION("A single namespace")
    {
        std::wstring str = LR"(
                            namespace N1
                            {
                            class A 
                            {
                            };
                            } )";
        Cda::File header{L"a.hpp", L"a.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        CHECK(res->name == L"N1::A"s);
    }

    SECTION("Two split namespaces")
    {
        std::wstring str = LR"(
                            namespace N1
                            {
                            namespace N2
                            {
                            class A 
                            {
                            };
                            }
                            } )";
        Cda::File header{L"a.hpp", L"a.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        CHECK(res->name == L"N1::N2::A"s);
    }

    SECTION("Two compact namespaces")
    {
        std::wstring str = LR"(
                            namespace N1::N2
                            {
                            class A 
                            {
                            };
                            } )";
        Cda::File header{L"a.hpp", L"a.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        CHECK(res->name == L"N1::N2::A"s);
    }
}
