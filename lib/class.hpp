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

struct Inheritance
{
    Visibility visibility;
    std::wstring type;
};

struct HeaderContent
{
    std::vector<MemberFunction> functions;
    std::vector<MemberVariable> variables;
    std::vector<Inheritance> inheritances;
};

struct Class
{
    Class(std::wstring name_, ClassFiles files_, HeaderContent header_content_);

    const std::wstring name;
    ClassFiles files;
    HeaderContent header_content;
};

} // namespace Cda
