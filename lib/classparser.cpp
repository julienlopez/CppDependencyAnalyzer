#include "classparser.hpp"

#include "utils/strings.hpp"

#include <map>

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

} // namespace

ClassFiles::ClassFiles(File header_file_, std::optional<File> source_file_)
    : header_file(std::move(header_file_))
    , source_file(std::move(source_file_))
{
}

Class::Class(ClassFiles files_)
    : files(std::move(files_))
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
    removeForwardDeclarations(files.header_file.m_lines);
    removePreprocessorLines(files.header_file.m_lines);
    removeEmptyNamespaces(files.header_file.m_lines);
    removeDocComments(files.header_file.m_lines);
    for(const auto& l : files.header_file.m_lines)
        std::wcout << L"\t" << l.number << ":\t" << l.content << std::endl;
    return {std::move(files)};
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
