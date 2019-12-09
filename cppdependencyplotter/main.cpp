#include "classparser.hpp"

#include <filesystem>
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
    std::wcout << L"loaded " << files.size() << L" files" << std::endl;
    for(const auto& f : files)
        std::wcout << f.fileName() << L"\n" << f.fullPath().stem() << std::endl;
    Cda::ClassParser::run(std::move(files));
    return 0;
}
