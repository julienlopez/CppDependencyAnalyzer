#include "file.hpp"

namespace Cda
{

File File::readFromFileSystem(std::wstring file_name, Path_t file)
{
    return File{std::move(file_name), std::move(file)};
}

const std::wstring& File::fileName() const
{
    return m_file_name;
}

auto File::fullPath() const -> const Path_t&
{
    return m_full_path;
}

File::File(std::wstring file_name, Path_t full_path)
    : m_file_name(std::move(file_name))
    , m_full_path(std::move(full_path))
{
}

} // namespace Cda
