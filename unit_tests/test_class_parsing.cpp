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
        auto res = Cda::ClassParser::parseClass(header);
        REQUIRE(res.header_content.functions.size() == 1);
        CHECK(res.header_content.functions.front().name == L"run");
        CHECK(res.header_content.functions.front().visibility == Cda::Visibility::Public);
        CHECK_FALSE(res.header_content.functions.front().is_const);
        REQUIRE(res.header_content.variables.size() == 1);
        CHECK(res.header_content.variables.front().name == L"m_a;");
        CHECK(res.header_content.variables.front().type == L"int");
        CHECK(res.header_content.variables.front().visibility == Cda::Visibility::Private);
        CHECK_FALSE(res.header_content.variables.front().is_const);
        CHECK_FALSE(res.header_content.variables.front().is_reference);
    }

    SECTION("Basic struct parsing")
    {
    }
}
