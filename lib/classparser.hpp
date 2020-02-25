#pragma once

#include <optional>

#include "class.hpp"

namespace Cda
{

class ClassParser
{
public:
    struct Parameters
    {
        bool keep_std_typed_variables = false;
        bool keep_boost_typed_variables = false;
    };

    explicit ClassParser(Parameters parameters = {false, false});

    ~ClassParser() = default;

    std::vector<Class> run(std::vector<File> files);

    std::optional<Class> parseClass(ClassFiles files);

private:
    Parameters m_parameters;
    Visibility m_current_visibility = Visibility::Private;

    static std::vector<ClassFiles> bunchFilesByClasses(std::vector<File> files);

    std::tuple<std::wstring, std::vector<Inheritance>, File::LineContainer_t::const_iterator,
               File::LineContainer_t::const_iterator>
    findClassesBoundariesAndName(const File::LineContainer_t& lines);

    HeaderContent parseHeaderContent(const std::wstring& class_name, File::LineContainer_t::const_iterator begin,
                                     File::LineContainer_t::const_iterator end);

    std::optional<MemberVariable> parseMemberVariable(std::wstring line) const;

    std::optional<MemberFunction> parseMemberFunction(std::wstring line) const;

    std::vector<Inheritance> findInheritances(std::wstring class_declaration_line) const;
};

} // namespace Cda
