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
    Class(ClassFiles files_);

    ClassFiles files;
};

class ClassParser
{
public:
    static std::vector<Class> run(std::vector<File> files);

private:
    static std::vector<ClassFiles> bunchFilesByClasses(std::vector<File> files);
};

} // namespace Cda
