#include "classparser.hpp"

#include "utils/strings.hpp"

#include <algorithm>
#include <map>
#include <stdexcept>

#include <iostream>

namespace Cda
{

namespace
{
    auto mapFilesToClasses(std::vector<File> files)
    {
        std::map<std::wstring, std::vector<File>> mapping;
        for(auto& f : files)
        {
            const auto stem = f.fullPath().stem().wstring();
            mapping[stem].push_back(std::move(f));
        }
        return mapping;
    }

    void removeForwardDeclarations(File::LineContainer_t& lines)
    {
        lines.erase(std::remove_if(begin(lines), end(lines),
                                   [](const File::Line& line) {
                                       return (Utils::Strings::startsWith(line.content, L"class ")
                                               || Utils::Strings::startsWith(line.content, L"struct "))
                                              && Utils::Strings::endsWith(line.content, L";");
                                   }),
                    end(lines));
    }

    void removePreprocessorLines(File::LineContainer_t& lines)
    {
        lines.erase(
            std::remove_if(begin(lines), end(lines),
                           [](const File::Line& line) { return Utils::Strings::startsWith(line.content, L"#"); }),
            end(lines));
    }

    void removeEmptyNamespaces(File::LineContainer_t& lines)
    {
        for(auto it = begin(lines); it != end(lines); ++it)
        {
            auto it2 = it;
            if(!Utils::Strings::startsWith(it2->content, L"namespace ")) continue;
            std::advance(it2, 1);
            if(it2 == end(lines) || it2->content != L"{") continue;
            std::advance(it2, 1);
            if(it2 == end(lines) || it2->content != L"}") continue;
            std::advance(it2, 1);
            if(it2 == end(lines)) continue;
            it = lines.erase(it, it2);
        }
    }

    void removeDocComments(File::LineContainer_t& lines)
    {
        for(auto it = begin(lines); it != end(lines); ++it)
        {
            auto it2 = it;
            if(!Utils::Strings::startsWith(it2->content, L"/**")) continue;
            std::advance(it2, 1);
            while(it2 != end(lines) && Utils::Strings::startsWith(it2->content, L"*"))
                std::advance(it2, 1);
            if(it2 == end(lines)) continue;
            it = lines.erase(it, it2);
        }
    }

    void removeBasicComments(File::LineContainer_t& lines)
    {
        lines.erase(
            std::remove_if(begin(lines), end(lines),
                           [](const File::Line& line) { return Utils::Strings::startsWith(line.content, L"//"); }),
            end(lines));
    }

    void mergeSplitLines(File::LineContainer_t& lines)
    {
        for(auto it = begin(lines); it != end(lines); ++it)
        {
            if(Utils::Strings::endsWith(it->content, L";")) continue;
            if(Utils::Strings::endsWith(it->content, L":")) continue;
            if(Utils::Strings::startsWith(it->content, L"{")) continue;
            if(Utils::Strings::startsWith(it->content, L"}")) continue;
            if(Utils::Strings::startsWith(it->content, L"namespace ")) continue;
            if(Utils::Strings::startsWith(it->content, L"class ")
               || Utils::Strings::startsWith(it->content, L"struct "))
            {
                if((it + 1) != end(lines) && (it + 1)->content == L"{") continue;
            }
            auto it_next = it + 1;
            if(it_next != end(lines))
            {
                it->content += L" " + it_next->content;
                lines.erase(it_next);
                if(it != begin(lines)) --it;
            }
        }
    }

    void cleanupHeaderFile(File::LineContainer_t& lines)
    {
        removeForwardDeclarations(lines);
        removePreprocessorLines(lines);
        removeEmptyNamespaces(lines);
        removeDocComments(lines);
        removeBasicComments(lines);
        mergeSplitLines(lines);
    }

    std::wstring cleanupClassName(std::wstring line)
    {
        if(Utils::Strings::startsWith(line, L"class ")) line.erase(begin(line), begin(line) + 6);
        if(Utils::Strings::startsWith(line, L"struct ")) line.erase(begin(line), begin(line) + 7);
        const auto it = std::find_if(begin(line), end(line), &isblank);
        line.erase(it, end(line));
        return line;
    }

    std::vector<File> filterFilesWithProperExtensions(std::vector<File> files)
    {
        std::vector<File> res;
        std::copy_if(
            std::make_move_iterator(begin(files)), std::make_move_iterator(end(files)), std::back_inserter(res),
            [](const File& f) { return f.fullPath().extension() == ".hpp" || f.fullPath().extension() == ".cpp"; });
        return res;
    }

    Visibility parseVisibility(const std::wstring& str)
    {
        if(str == L"public") return Visibility::Public;
        if(str == L"protected") return Visibility::Protected;
        return Visibility::Private;
    }

} // namespace

ClassFiles::ClassFiles(File header_file_, std::optional<File> source_file_)
    : header_file(std::move(header_file_))
    , source_file(std::move(source_file_))
{
}

Class::Class(std::wstring name_, ClassFiles files_, HeaderContent header_content_)
    : name(std::move(name_))
    , files(std::move(files_))
    , header_content(std::move(header_content_))
{
}

ClassParser::ClassParser(Parameters parameters)
    : m_parameters(std::move(parameters))
{
}

std::vector<Class> ClassParser::run(std::vector<File> files)
{
    auto class_files = bunchFilesByClasses(filterFilesWithProperExtensions(std::move(files)));
    std::vector<Class> res;
    for(auto& f : class_files)
        res.emplace_back(parseClass(std::move(f)));
    return res;
}

Class ClassParser::parseClass(ClassFiles files)
{
    cleanupHeaderFile(files.header_file.m_lines);
    auto[name, inheritances, it_begin, it_end] = findClassesBoundariesAndName(files.header_file.m_lines);
    auto content = parseHeaderContent(name, it_begin, it_end);
    content.inheritances = std::move(inheritances);
    return {std::move(name), std::move(files), std::move(content)};
}

std::tuple<std::wstring, std::vector<Inheritance>, File::LineContainer_t::const_iterator,
           File::LineContainer_t::const_iterator>
ClassParser::findClassesBoundariesAndName(const File::LineContainer_t& lines)
{
    auto it_begin = std::find_if(begin(lines), end(lines), [](const File::Line& line) {
        return Utils::Strings::startsWith(line.content, L"class ")
               || Utils::Strings::startsWith(line.content, L"struct ");
    });
    if(it_begin == end(lines)) throw std::runtime_error("unable to find the start of the class");
    if(Utils::Strings::startsWith(it_begin->content, L"class ")) m_current_visibility = Visibility::Private;
    if(Utils::Strings::startsWith(it_begin->content, L"struct ")) m_current_visibility = Visibility::Public;
    auto name = it_begin->content;
    const auto inheritances = findInheritances(it_begin->content);
    std::advance(it_begin, 1);
    if(it_begin == end(lines) || it_begin->content != L"{")
        throw std::runtime_error("unable to find the start of the class");
    std::advance(it_begin, 1);
    if(it_begin == end(lines)) throw std::runtime_error("unable to find the start of the class");
    const auto it_end
        = std::find_if(it_begin, end(lines), [](const File::Line& line) { return line.content == L"};"; });
    if(it_end == end(lines)) throw std::runtime_error("unable to find the end of the class");
    return {cleanupClassName(std::move(name)), inheritances, it_begin, it_end};
}

std::vector<ClassFiles> ClassParser::bunchFilesByClasses(std::vector<File> files)
{
    std::vector<ClassFiles> res;
    for(auto& p : mapFilesToClasses(std::move(files)))
    {
        if(p.second.size() > 2) throw std::runtime_error("unable to handle classes with more than two files");
        std::optional<File> header_file = std::nullopt;
        std::optional<File> source_file = std::nullopt;
        for(auto& f : p.second)
        {
            if(f.fullPath().extension() == ".hpp") header_file.emplace(std::move(f));
            if(f.fullPath().extension() == ".cpp") source_file.emplace(std::move(f));
        }
        if(!header_file) throw std::runtime_error("unable to handle classes with no header file");
        res.emplace_back(std::move(*header_file), std::move(source_file));
    }
    return res;
}

HeaderContent ClassParser::parseHeaderContent(const std::wstring& class_name,
                                              File::LineContainer_t::const_iterator begin,
                                              File::LineContainer_t::const_iterator end)
{
    HeaderContent res;
    for(auto it = begin; it != end; ++it)
    {
        auto current_line = it->content;
        if(Utils::Strings::startsWith(current_line, L"using")) continue;
        if(current_line.find(class_name + L"(") != std::wstring::npos) continue;
        if(current_line == L"public:")
            m_current_visibility = Visibility::Public;
        else if(current_line == L"protected:")
            m_current_visibility = Visibility::Protected;
        else if(current_line == L"private:")
            m_current_visibility = Visibility::Private;
        else

        {
            const auto it2 = current_line.find(L"(");
            if(it2 == std::wstring::npos)
            {
                auto var = parseMemberVariable(current_line);
                if(var)
                {
                    if(!m_parameters.keep_std_typed_variables && Utils::Strings::startsWith(var->type, L"std::"))
                        continue;
                    if(!m_parameters.keep_boost_typed_variables && Utils::Strings::startsWith(var->type, L"boost::"))
                        continue;
                    res.variables.push_back(std::move(*var));
                }
            }
            else
            {
                auto fct = parseMemberFunction(current_line);
                if(fct) res.functions.push_back(std::move(*fct));
            }
        }
    }
    return res;
}

std::optional<MemberVariable> ClassParser::parseMemberVariable(std::wstring line) const
{
    if(line.find(L")") != std::wstring::npos || line.find(L",") != std::wstring::npos
       || line.find(L"[[nodiscard]]") != std::wstring::npos)
    {
        std::wcerr << L"can't parse member variable " << line << L" (1)" << std::endl;
        return std::nullopt;
    }
    bool is_const = false;
    if(Utils::Strings::startsWith(line, L"const "))
    {
        is_const = true;
        line.erase(0, 6);
    }
    auto parts = Utils::Strings::split(line, L' ');
    if(parts.size() != 2)
    {
        std::wcerr << L"can't parse member variable " << line << L" (2)" << std::endl;
        return std::nullopt;
    }
    for(auto& p : parts)
        p = Utils::Strings::trim(p);
    bool is_reference = false;
    if(Utils::Strings::endsWith(parts[0], L"&"))
    {
        is_reference = true;
        parts[0].pop_back();
    }
    if(Utils::Strings::startsWith(parts[0], L"std::unique_ptr<") && Utils::Strings::endsWith(parts[0], L">"))
    {
        parts[0] = parts[0].substr(16, parts[0].size() - 17);
    }
    parts[1].pop_back();
    return MemberVariable{m_current_visibility, parts[0], parts[1], is_reference, is_const};
}

std::optional<MemberFunction> ClassParser::parseMemberFunction(const std::wstring& line) const
{
    const auto is_const = Utils::Strings::endsWith(line, L" const;") || Utils::Strings::endsWith(line, L" const = 0;");
    auto parts = Utils::Strings::split(line, L'(');
    if(parts.size() != 2)
    {
        std::wcerr << L"can't parse member function " << line << std::endl;
        return std::nullopt;
    }
    parts = Utils::Strings::split(parts[0], L' ');
    if(!parts.empty() && parts[0] == L"[[nodiscard]]") parts.erase(begin(parts));
    if(parts.size() != 2)
    {
        std::wcerr << L"can't parse member function " << line << std::endl;
        return std::nullopt;
    }
    for(auto& p : parts)
        p = Utils::Strings::trim(p);
    return MemberFunction{m_current_visibility, parts[1], is_const};
}

std::vector<Inheritance> ClassParser::findInheritances(std::wstring class_declaration_line) const
{
    const auto it = find(begin(class_declaration_line), end(class_declaration_line), ':');
    if(it == end(class_declaration_line)) return {};
    class_declaration_line.erase(begin(class_declaration_line), it + 1);
    class_declaration_line = Utils::Strings::trim(class_declaration_line);
    std::vector<Inheritance> res;
    auto inheritances = Utils::Strings::split(class_declaration_line, ',');
    for(auto& inheritance : inheritances)
    {
        inheritance = Utils::Strings::trim(inheritance);
        const auto parts = Utils::Strings::split(inheritance, ' ');
        if(parts.size() == 2)
        {
            if(!m_parameters.keep_std_typed_variables && Utils::Strings::startsWith(parts[1], L"std::")) continue;
            if(!m_parameters.keep_boost_typed_variables && Utils::Strings::startsWith(parts[1], L"boost::")) continue;
            res.push_back({parseVisibility(parts[0]), parts[1]});
        }
    }
    return res;
}

} // namespace Cda
