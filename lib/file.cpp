#include "file.hpp"

#include <fstream>

namespace Cda
{

namespace
{
    auto readAllLinesInFile(const std::filesystem::path& file_path)
    {
        std::vector<File::Line> res;
        std::wstring line;
        std::wifstream stream{file_path};
        std::size_t cpt = 0;
        while(std::getline(stream, line))
        {
            if(!line.empty()) res.push_back({cpt, std::move(line)});
            cpt++;
        }
        return res;
    }

} // namespace

File::File(std::wstring file_name, Path_t full_path, LineContainer_t lines)
    : m_file_name(std::move(file_name))
    , m_full_path(std::move(full_path))
    , m_lines(std::move(lines))
{
}

File File::readFromFileSystem(std::wstring file_name, Path_t file)
{
    auto lines = readAllLinesInFile(file);
    return File{std::move(file_name), std::move(file), std::move(lines)};
}

const std::wstring& File::fileName() const
{
    return m_file_name;
}

auto File::fullPath() const -> const Path_t&
{
    return m_full_path;
}

} // namespace Cda
