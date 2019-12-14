#pragma once

#include "class.hpp"

#include <boost/graph/adjacency_list.hpp>

namespace Cda
{

class GraphBuilder
{
public:
    struct type_t
    {
        using kind = boost::edge_property_tag;
    };

    enum class Type
    {
        Use,
        Inherits
    };

    using VertexProperties = boost::property<boost::vertex_name_t, std::wstring>;
    using EdgeProperties = boost::property<type_t, Type>;
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

} // namespace Cda
