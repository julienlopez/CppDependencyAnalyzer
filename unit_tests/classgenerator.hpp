#pragma once

#include "class.hpp"

namespace Testing
{

class ClassGenerator
{
public:
    explicit ClassGenerator(std::wstring class_name = L"A", std::wstring header_file_name = L"a.hpp");

    ~ClassGenerator() = default;

    operator Cda::ClassFiles() const;

    ClassGenerator& addLine(std::wstring line);

private:
    const std::wstring m_class_name;
    const std::wstring m_header_file_name;
    std::wstring m_content;
};

std::vector<Cda::File::Line> linesFromString(const std::wstring& str);

} // namespace Testing
