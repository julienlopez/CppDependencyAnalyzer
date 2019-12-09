#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Cda
{

class File
{
public:
    using Path_t = std::filesystem::path;

    /**
    * @pre is_regular_file(file)
    */
    static File readFromFileSystem(std::wstring file_name, Path_t file);

    ~File() = default;

    struct Line
    {
        std::size_t number;
        std::wstring content;
    };

    const std::wstring& fileName() const;

    const Path_t& fullPath() const;

private:
    explicit File(std::wstring file_name, Path_t full_path);

    const std::wstring m_file_name;
    const Path_t m_full_path;
    std::vector<Line> m_lines;
};

} // namespace Cda
