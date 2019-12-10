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

struct Class
{
    Class(std::wstring name_, ClassFiles files_);

    const std::wstring name;
    ClassFiles files;
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
