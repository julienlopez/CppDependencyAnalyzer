#include "classgenerator.hpp"

#include "utils/strings.hpp"

namespace Testing
{
ClassGenerator::ClassGenerator(std::wstring class_name, std::wstring header_file_name)
    : m_class_name(std::move(class_name))
    , m_header_file_name(std::move(header_file_name))
{
}

ClassGenerator::operator Cda::ClassFiles() const
{
    auto full_text = L"class " + m_class_name + L"\n{\n";
    full_text += m_content;
    full_text += L"};\n";
    return Cda::File{m_header_file_name, m_header_file_name, linesFromString(full_text)};
}

ClassGenerator& ClassGenerator::addLine(std::wstring line)
{
    m_content += line + L"\n";
    return *this;
}

std::vector<Cda::File::Line> linesFromString(const std::wstring& str)
{
    std::vector<Cda::File::Line> res;
    auto parts = Cda::Utils::Strings::split(str, L'\n');
    std::size_t cpt = 0;
    for(auto& line : parts)
        res.push_back({cpt++, Cda::Utils::Strings::trim(std::move(line))});
    return res;
}

} // namespace Testing
