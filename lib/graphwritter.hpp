#pragma once

#include <iosfwd>

namespace Cda
{

class GraphBuilder;

class GraphWritter
{
public:
    GraphWritter(std::ostream& output);

    ~GraphWritter() = default;

    void printGraph(const GraphBuilder& graph) const;

private:
    std::ostream& m_output;
};

} // namespace Cda
