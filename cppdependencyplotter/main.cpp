#include "classparser.hpp"
#include "graphbuilder.hpp"
#include "graphwritter.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <gsl/gsl_assert>

namespace
{

template <class ITER1, class ITER2, class OUTPUT_ITER>
OUTPUT_ITER copy_difference(ITER1 begin1, ITER1 end1, ITER2 begin2, ITER2 end2, OUTPUT_ITER out)
{
    auto[it1, it2] = std::mismatch(begin1, end1, begin2);
    if(it1 == end1 && it2 == end2) return out;
    if(it1 == end1)
        return std::copy(it2, end2, out);
    else
        return std::copy(it1, end1, out);
}

std::wstring cleanRootFromFileName(const std::wstring& directory, const std::wstring& file_path)
{
    std::wstring res;
    copy_difference(begin(directory), end(directory), begin(file_path), end(file_path), std::back_inserter(res));
    if(!res.empty() && (res.front() == L'\\' || res.front() == L'/')) res.erase(0, 1);
    return res;
}

/**
 * @pre std::filesystem::is_directory(directory)
 */
std::vector<Cda::File> loadFiles(const std::filesystem::path& directory)
{
    Expects(std::filesystem::is_directory(directory));
    std::vector<Cda::File> res;
    for(std::filesystem::directory_iterator it{directory}; it != std::filesystem::directory_iterator{}; ++it)
    {
        const auto filename = cleanRootFromFileName(directory.wstring(), it->path().wstring());
        res.push_back(Cda::File::readFromFileSystem(filename, *it));
    }
    return res;
}

} // namespace

int main(int argc, char* argv[])
{
    if(argc != 2 || !std::filesystem::is_directory(argv[1]))
    {
        std::wcerr << L"expected usage : cpp_dependency_plotter <dir>" << std::endl;
        return 1;
    }
    auto files = loadFiles(argv[1]);
    const auto classes = Cda::ClassParser::run(std::move(files));

    for(const auto& c : classes)
    {
        std::wcout << c.name << L":\n" << L"member functions:\n";
        for(const auto& f : c.header_content.functions)
        {
            std::wcout << L"\t" << f.name << L"()";
            if(f.is_const) std::wcout << L" (const)";
            std::wcout << std::endl;
        }
        std::wcout << L"member variables:\n";
        for(const auto& v : c.header_content.variables)
        {
            std::wcout << L"\t";
            if(v.is_const) std::wcout << L"const ";
            std::wcout << v.type;
            if(v.is_reference) std::wcout << L"&";
            std::wcout << L" " << v.name;
            std::wcout << std::endl;
        }
    }

    Cda::GraphBuilder graph;
    graph.buildGraph(classes);
    std::ofstream dotfile("out.dot");
    Cda::GraphWritter(dotfile).printGraph(graph);

    return 0;
}
