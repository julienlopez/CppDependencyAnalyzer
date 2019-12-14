#include <catch2/catch.hpp>

#include "classparser.hpp"

#include "utils/strings.hpp"

namespace
{

auto linesFromString(const std::wstring& str)
{
    std::vector<Cda::File::Line> res;
    auto parts = Cda::Utils::Strings::split(str, L'\n');
    std::size_t cpt = 0;
    for(auto& line : parts)
        res.push_back({cpt++, Cda::Utils::Strings::trim(std::move(line))});
    return res;
}

} // namespace

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
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
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
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
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
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
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
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
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
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
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
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
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
        REQUIRE(res.header_content.functions.empty());
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a");
        CHECK(res.header_content.variables.front().type == L"std::string");
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
        REQUIRE(res.header_content.functions.empty());
        REQUIRE(res.header_content.variables.empty());
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
        REQUIRE(res.name == L"A");
        REQUIRE(res.header_content.inheritances.size() == 1);
        CHECK(res.header_content.inheritances.front().type == L"B");
        CHECK(res.header_content.inheritances.front().visibility == Cda::Visibility::Public);
    }
}
