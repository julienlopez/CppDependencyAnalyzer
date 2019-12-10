#pragma once

#include "class.hpp"

#include <boost/graph/adjacency_list.hpp>

namespace Cda
{

class GraphBuilder
{
public:
    using VertexProperties = boost::property<boost::vertex_name_t, std::wstring>;
    using EdgeProperties = boost::property<boost::edge_name_t, std::wstring>;
    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperties, EdgeProperties>;

    GraphBuilder() = default;

    ~GraphBuilder() = default;

    void buildGraph(const std::vector<Class>& classes);

    const Graph& graph() const;

    std::wstring vertexName(const Graph::vertex_descriptor v) const;

private:
    using VertexMapping_t = std::map<std::wstring, GraphBuilder::Graph::vertex_descriptor>;

    Graph m_graph;
    VertexMapping_t m_vertex_mapping;

    Graph::vertex_descriptor findOrAddVertex(const std::wstring& name);
};

std::string get(const GraphBuilder& g, const GraphBuilder::Graph::vertex_descriptor v);

} // namespace Cda
