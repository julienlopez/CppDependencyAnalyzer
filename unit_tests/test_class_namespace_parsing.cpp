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

    SECTION("parsing two classes in a row doesn't have any side effect namespace-wise")
    {
        std::wstring str1 = LR"(
                            namespace N1
                            {
                            class A
                            {
                            };
                            } )";

        std::wstring str2 = LR"(
                            namespace N2
                            {
                            class B
                            {
                            };
                            } )";
        Cda::ClassParser parser{};
        const auto res1 = parser.parseClass(Cda::File{L"a.hpp", L"a.hpp", linesFromString(str1)});
        const auto res2 = parser.parseClass(Cda::File{L"b.hpp", L"b.hpp", linesFromString(str2)});
        REQUIRE(res1);
        CHECK(res1->name == L"N1::A"s);
        REQUIRE(res2);
        CHECK(res2->name == L"N2::B"s);
    }
}

TEST_CASE("Figuring out the full type of a variable member inside namespaces", "[ClassParser]")
{

    SECTION("same namespace namespace")
    {
        std::wstring str = LR"(
                            namespace N1
                            {
                            class A
                            {
                                B m_b;
                            };
                            } )";
        Cda::File header{L"a.hpp", L"a.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        CHECK(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().type == L"N1::B"s);
    }
}
