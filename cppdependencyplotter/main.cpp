#include "file.hpp"

#include <filesystem>
#include <iostream>

#include <gsl/gsl_assert>

namespace
{

/**
 * @pre std::filesystem::is_directory(directory)
 */
std::vector<Cda::File> loadFiles(const std::filesystem::path& directory)
{
    Expects(std::filesystem::is_directory(directory));
    std::vector<Cda::File> res;
    for(std::filesystem::directory_iterator it{directory} ; it != std::filesystem::directory_iterator{}; ++it)
    {
        std::wcout << *it << std::endl;
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
    return 0;
}
