#include "classparser.hpp"

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

} // namespace

ClassFiles::ClassFiles(File header_file_, std::optional<File> source_file_)
    :

    header_file(std::move(header_file_))
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
        res.emplace_back(std::move(f));
    return res;
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
