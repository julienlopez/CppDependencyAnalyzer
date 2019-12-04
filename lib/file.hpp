#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Cda
{

class File
{
public:
    static File readFromFileSystem(std::wstring file_name, const std::filesystem::path& file);

    ~File() = default;

    struct Line
    {
        std::size_t number;
        std::wstring content;
	};

private:
    explicit File(std::wstring file_name);

    std::wstring m_file_name;
    std::vector<Line> m_lines;
};

} // namespace Cda
