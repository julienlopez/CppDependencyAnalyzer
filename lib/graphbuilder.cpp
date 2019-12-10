#include "graphbuilder.hpp"

#include <iostream>

namespace Cda
{

namespace
{
    using VertexMapping_t = std::map<std::wstring, GraphBuilder::Graph::vertex_descriptor>;

    std::optional<GraphBuilder::Graph::vertex_descriptor> findVertex(const VertexMapping_t& mapping,
                                                                     const std::wstring& name)
    {
        auto it = mapping.find(name);
        if(it == end(mapping))
        {
            std::wcerr << L"Unable to find " << name << std::endl;
            return std::nullopt;
        }
        return it->second;
    }

} // namespace

auto GraphBuilder::buildGraph(const std::vector<Class>& classes) -> Graph
{
    Graph g;
    VertexMapping_t mapping;
    for(const auto& c : classes)
    {
        mapping[c.name] = boost::add_vertex(VertexProperties{c.name}, g);
    }

    for(const auto& c : classes)
        for(const auto& var : c.header_content.variables)
        {
            const auto u = findVertex(mapping, c.name);
            const auto v = findVertex(mapping, var.type);
            if(u && v) boost::add_edge(*u, *v, EdgeProperties{L"uses"}, g);
        }

    return g;
}

} // namespace Cda
