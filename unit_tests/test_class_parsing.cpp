#include <catch2/catch.hpp>

#include "classparser.hpp"

#include "classgenerator.hpp"

using namespace Testing;

TEST_CASE("Basic uses of ClassParser", "[ClassParser]")
{

    SECTION("Basic class parsing")
    {
        std::wstring str = LR"(class A 
                            {
                                public:
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res->header_content.functions.front().is_const);
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"int");
        CHECK(res->header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res->header_content.variables.front().is_const);
        CHECK_FALSE(res->header_content.variables.front().is_reference);
    }

    SECTION("Basic struct parsing")
    {
        std::wstring str = LR"(struct A 
                            {
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res->header_content.functions.front().is_const);
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"int");
        CHECK(res->header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res->header_content.variables.front().is_const);
        CHECK_FALSE(res->header_content.variables.front().is_reference);
    }

    SECTION("Class parsing discards doc comments")
    {
        std::wstring str = LR"(class A 
                            {
                                public:
                                    /**
                                    * do stuff
                                    */
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res->header_content.functions.front().is_const);
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"int");
        CHECK(res->header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res->header_content.variables.front().is_const);
        CHECK_FALSE(res->header_content.variables.front().is_reference);
    }

    SECTION("Class parsing discards basic comments (1)")
    {
        std::wstring str = LR"(class A 
                            {
                                public:
                                    // some comment
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res->header_content.functions.front().is_const);
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"int");
        CHECK(res->header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res->header_content.variables.front().is_const);
        CHECK_FALSE(res->header_content.variables.front().is_reference);
    }

    SECTION("Class parsing discards basic comments (2)")
    {
        std::wstring str = LR"(class A 
                            {
                                public:
                                    // some_comment
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res->header_content.functions.front().is_const);
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"int");
        CHECK(res->header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res->header_content.variables.front().is_const);
        CHECK_FALSE(res->header_content.variables.front().is_reference);
    }

    SECTION("Class parsing multi-line method handling")
    {
        std::wstring str = LR"(class A 
                            {
                                public:
                                    int run(
                                        int a,
                                        int b);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->header_content.functions.size() == 1);
        CHECK(res->header_content.functions.front().name == L"run");
        CHECK(res->header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res->header_content.functions.front().is_const);
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"int");
        CHECK(res->header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res->header_content.variables.front().is_const);
        CHECK_FALSE(res->header_content.variables.front().is_reference);
    }

    SECTION("handling variables with std types")
    {
        std::wstring str = LR"(class A 
                            {
                                private:
                                    std::string m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser({true}).parseClass(header);
        REQUIRE(res->header_content.functions.empty());
        REQUIRE(res->header_content.variables.size() == 1);
        CHECK(res->header_content.variables.front().name == L"m_a");
        CHECK(res->header_content.variables.front().type == L"std::string");
    }

    SECTION("disabling variables with std types")
    {
        std::wstring str = LR"(class A 
                            {
                                private:
                                    std::string m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser({false}).parseClass(header);
        REQUIRE(res->header_content.functions.empty());
        REQUIRE(res->header_content.variables.empty());
    }

    SECTION("Class parsing with one public inheritance")
    {
        std::wstring str = LR"(class A : public B
                            {
                                public:
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->name == L"A");
        REQUIRE(res->header_content.inheritances.size() == 1);
        CHECK(res->header_content.inheritances.front().type == L"B");
        CHECK(res->header_content.inheritances.front().visibility == Cda::Visibility::Public);
    }

    SECTION("Class parsing with two public inheritances on a single line")
    {
        std::wstring str = LR"(class A : public B, private C
                            {
                                public:
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->name == L"A");
        REQUIRE(res->header_content.inheritances.size() == 2);
        CHECK(res->header_content.inheritances.front().type == L"B");
        CHECK(res->header_content.inheritances.front().visibility == Cda::Visibility::Public);
        CHECK(res->header_content.inheritances.back().type == L"C");
        CHECK(res->header_content.inheritances.back().visibility == Cda::Visibility::Private);
    }

    SECTION("Class parsing with two public inheritances on a two line")
    {
        std::wstring str = LR"(class A : public B, 
                                         private C
                            {
                                public:
                                    int run(int i);
                                private:
                                    int m_a;
                            }; )";
        Cda::File header{L"file.hpp", L"file.hpp", linesFromString(str)};
        auto res = Cda::ClassParser().parseClass(header);
        REQUIRE(res);
        REQUIRE(res->name == L"A");
        REQUIRE(res->header_content.inheritances.size() == 2);
        CHECK(res->header_content.inheritances.front().type == L"B");
        CHECK(res->header_content.inheritances.front().visibility == Cda::Visibility::Public);
        CHECK(res->header_content.inheritances.back().type == L"C");
        CHECK(res->header_content.inheritances.back().visibility == Cda::Visibility::Private);
    }
}

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
