#include <catch2/catch.hpp>

#include "graphbuilder.hpp"
#include "graphwritter.hpp"

#include "utils/strings.hpp"

#include <cwctype>

std::string writeGraph(const std::vector<Cda::Class>& classes)
{
    std::stringstream ss;
    auto graph = Cda::GraphBuilder();
    graph.buildGraph(classes);
    Cda::GraphWritter(ss).printGraph(graph);
    return ss.str();
}

Cda::Class packClass(const std::wstring name, Cda::HeaderContent header_content)
{
    std::wstring lc_name;
    std::transform(begin(name), end(name), std::back_inserter(lc_name), &std::towlower);
    return {name, {Cda::File{lc_name + L".hpp", lc_name + L".hpp", {}}, std::nullopt}, std::move(header_content)};
}

using namespace Cda::Utils::Strings;

TEST_CASE("Basic uses of GraphWritter", "[GraphWritter]")
{

    SECTION("Basic class parsing")
    {
        std::vector<Cda::Class> classes;
        Cda::MemberVariable fct{Cda::Visibility::Private, L"C", L"m_c", false, false};
        Cda::Inheritance inheritance{Cda::Visibility::Public, L"B"};
        classes.push_back(packClass(L"A", Cda::HeaderContent{{}, {fct}, {inheritance}}));
        const auto res = writeGraph(classes);
        const auto lines = split(res, L'\n');
        REQUIRE(lines.size() == 9);
        CHECK(startsWith(lines[0], "digraph"));
        CHECK(startsWith(lines[1], "graph ["));
        CHECK(startsWith(lines[2], "node ["));
        CHECK(startsWith(lines[3], "0[label=\"<f0>A |"));
        CHECK(startsWith(lines[4], "1[label=\"<f0>C |"));
        CHECK(startsWith(lines[5], "2[label=\"<f0>B |"));
        CHECK(lines[6] == "0->1 [arrowhead=normal];");
        CHECK(lines[7] == "0->2 [arrowhead=onormal];");
        CHECK(lines[8] == "}");
    }
}