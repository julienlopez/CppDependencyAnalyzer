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

    static Graph buildGraph(const std::vector<Class>& classes);
};

} // namespace Cda
