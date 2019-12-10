#include "classparser.hpp"

#include "utils/strings.hpp"

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

    void cleanupHeaderFile(File::LineContainer_t& lines)
    {
        removeForwardDeclarations(lines);
        removePreprocessorLines(lines);
        removeEmptyNamespaces(lines);
        removeDocComments(lines);
    }

    std::wstring cleanupClassName(std::wstring line)
    {
        if(Utils::Strings::startsWith(line, L"class ")) line.erase(begin(line), begin(line) + 6);
        const auto it = std::find_if(begin(line), end(line), &isblank);
        line.erase(it, end(line));
        return line;
    }

    std::tuple<std::wstring, File::LineContainer_t::const_iterator, File::LineContainer_t::const_iterator>
    findClassesBoundariesAndName(const File::LineContainer_t& lines)
    {
        auto it_begin = std::find_if(begin(lines), end(lines), [](const File::Line& line) {
            return Utils::Strings::startsWith(line.content, L"class ");
        });
        if(it_begin == end(lines)) throw std::runtime_error("unable to find the start of the class");
        auto name = it_begin->content;
        std::advance(it_begin, 1);
        if(it_begin == end(lines) || it_begin->content != L"{")
            throw std::runtime_error("unable to find the start of the class");
        std::advance(it_begin, 1);
        if(it_begin == end(lines)) throw std::runtime_error("unable to find the start of the class");
        const auto it_end
            = std::find_if(it_begin, end(lines), [](const File::Line& line) { return line.content == L"};"; });
        if(it_end == end(lines)) throw std::runtime_error("unable to find the end of the class");
        return {cleanupClassName(std::move(name)), it_begin, it_end};
    }

    std::optional<MemberVariable> parseMemberVariable(const std::wstring& line, Visibility current_visibility)
    {
        auto parts = Utils::Strings::split(line, L' ');
        if(parts.size() != 2)
        {
            std::wcerr << L"can't parse member variable " << line << std::endl;
            return std::nullopt;
        }
        for(auto& p : parts)
            p = Utils::Strings::trim(p);
        bool is_reference = false;
        bool is_const = false;
        if(Utils::Strings::endsWith(parts[0], L"&"))
        {
            is_reference = true;
            parts[0].pop_back();
        }
        if(Utils::Strings::startsWith(parts[0], L"const "))
        {
            is_const = true;
            parts[0].erase(0, 6);
        }
        return MemberVariable{current_visibility, parts[0], parts[1], is_reference, is_const};
    }

    HeaderContent parseHeaderContent(const std::wstring& class_name, File::LineContainer_t::const_iterator begin,
                                     File::LineContainer_t::const_iterator end)
    {
        HeaderContent res;
        auto current_visibility = Visibility::Private;
        for(auto it = begin; it != end; ++it)
        {
            auto current_line = it->content;
            if(Utils::Strings::startsWith(current_line, L"using")) continue;
            if(current_line.find(class_name + L"(") != std::wstring::npos) continue;
            if(current_line == L"public:")
                current_visibility = Visibility::Public;
            else if(current_line == L"protected:")
                current_visibility = Visibility::Protected;
            else if(current_line == L"private:")
                current_visibility = Visibility::Private;
            else

            {
                const auto it2 = current_line.find(L"(");
                if(it2 == std::wstring::npos)
                {
                    const auto var = parseMemberVariable(current_line, current_visibility);
                    if(var) res.variables.push_back(std::move(*var));
                }
                else
                {
                    std::wcout << "\t\tfnc: " << current_line << std::endl;
                }
            }
        }
        return res;
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

std::vector<Class> ClassParser::run(std::vector<File> files)
{
    auto class_files = bunchFilesByClasses(std::move(files));
    std::vector<Class> res;
    for(auto& f : class_files)
        res.emplace_back(parseClass(std::move(f)));
    return res;
}

Class ClassParser::parseClass(ClassFiles files)
{
    cleanupHeaderFile(files.header_file.m_lines);
    auto[name, it_begin, it_end] = findClassesBoundariesAndName(files.header_file.m_lines);
    std::wcout << files.header_file.fullPath() << std::endl;
    std::wcout << name << std::endl;
    for(const auto& l : files.header_file.m_lines)
        std::wcout << L"\t" << l.number << ":\t" << l.content << std::endl;
    auto content = parseHeaderContent(name, it_begin, it_end);
    return {std::move(name), std::move(files), std::move(content)};
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

} // namespace Cda
