#include "graphwritter.hpp"

#include "graphbuilder.hpp"

#include "utils/strings.hpp"

#include <boost/graph/graphviz.hpp>

namespace Cda
{

namespace
{

    class VertexWriter
    {
    public:
        VertexWriter(const Cda::GraphBuilder& graph)
            : m_graph(graph)
        {
        }

        template <class Vertex> void operator()(std::ostream& out, const Vertex& v) const
        {
            out << "[label=\"<f0>" << Cda::Utils::Strings::convert(m_graph.vertexName(v)) << " | <f1> test()\"]";
        }

    private:
        const Cda::GraphBuilder& m_graph;
    };

    class EdgeWriter
    {
    public:
        EdgeWriter(const Cda::GraphBuilder& graph)
            : m_graph(graph)
        {
        }

        template <class Edge> void operator()(std::ostream& out, const Edge& e) const
        {
            const auto map = boost::get(GraphBuilder::type_t{}, m_graph.graph());
            if(map(e) == GraphBuilder::Type::Inherits)
                out << "[arrowhead=onormal]"; // inheritance
            else
                out << "[arrowhead=normal]"; // classic use
        }

    private:
        const Cda::GraphBuilder& m_graph;
    };

    class GraphWriter
    {
    public:
        GraphWriter() = default;

        void operator()(std::ostream& out) const
        {
            out << "graph [rankdir = \"LR\" nodesep = 2];\nnode [shape=record,width=.1,height=.1];";
        }
    };
}

GraphWritter::GraphWritter(std::ostream& output)
    : m_output(output)
{
}

void GraphWritter::printGraph(const GraphBuilder& graph) const
{
    boost::write_graphviz(m_output, graph.graph(), VertexWriter{graph}, EdgeWriter{graph}, GraphWriter{});
}

} // namespace Cda
