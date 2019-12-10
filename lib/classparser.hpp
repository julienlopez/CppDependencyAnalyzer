#pragma once

#include <optional>

#include "file.hpp"

namespace Cda
{

struct ClassFiles
{
    ClassFiles(File header_file_, std::optional<File> source_file_ = std::nullopt);

    File header_file;
    std::optional<File> source_file;
};

enum class Visibility
{
    Private,
    Protected,
    Public
};

struct MemberFunction
{
    Visibility visibility;
    // std::wstring return_type;
    std::wstring name;
    bool is_const;
};

struct MemberVariable
{
    Visibility visibility;
    std::wstring type;
    std::wstring name;
    bool is_reference;
    bool is_const;
};

struct HeaderContent
{
    std::vector<MemberFunction> functions;
    std::vector<MemberVariable> variables;
};

struct Class
{
    Class(std::wstring name_, ClassFiles files_, HeaderContent header_content_);

    const std::wstring name;
    ClassFiles files;
    HeaderContent header_content;
};

class ClassParser
{
public:
    static std::vector<Class> run(std::vector<File> files);

    static Class parseClass(ClassFiles files);

private:
    static std::vector<ClassFiles> bunchFilesByClasses(std::vector<File> files);
};

} // namespace Cda
