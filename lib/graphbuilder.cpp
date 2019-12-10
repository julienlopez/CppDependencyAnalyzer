#include "graphbuilder.hpp"

#include "utils/strings.hpp"

#include <iostream>

namespace Cda
{

void GraphBuilder::buildGraph(const std::vector<Class>& classes)
{
    for(const auto& c : classes)
    {
        m_vertex_mapping[c.name] = boost::add_vertex(VertexProperties{c.name}, m_graph);
    }

    for(const auto& c : classes)
        for(const auto& var : c.header_content.variables)
        {
            const auto u = findOrAddVertex(c.name);
            const auto v = findOrAddVertex(var.type);
            boost::add_edge(u, v, EdgeProperties{L"uses"}, m_graph);
        }
}

auto GraphBuilder::graph() const -> const Graph&
{
    return m_graph;
}

auto GraphBuilder::findOrAddVertex(const std::wstring& name) -> Graph::vertex_descriptor
{
    auto it = m_vertex_mapping.find(name);
    if(it == end(m_vertex_mapping))
    {
        std::wcerr << L"Unable to find " << name << std::endl;
        const auto res = boost::add_vertex(VertexProperties{name}, m_graph);
        m_vertex_mapping[name] = res;
        return res;
    }
    return it->second;
}

std::wstring GraphBuilder::vertexName(const Graph::vertex_descriptor v) const
{
    return m_graph.m_vertices[v].m_property.m_value;
}

// TODO find a better way?
std::string get(const GraphBuilder& g, const GraphBuilder::Graph::vertex_descriptor v)
{
    return Utils::Strings::convert(g.vertexName(v));
}

} // namespace Cda
