#include "file.hpp"

namespace Cda
{

File File::readFromFileSystem(std::wstring file_name, const std::filesystem::path& file)
{
    return File{std::move(file_name)};
}

File::File(std::wstring file_name)
    : m_file_name(std::move(file_name))
{
}

} // namespace Cda
