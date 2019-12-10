#pragma once

#include <optional>

#include "class.hpp"

namespace Cda
{

class ClassParser
{
public:
    static std::vector<Class> run(std::vector<File> files);

    static Class parseClass(ClassFiles files);

private:
    static std::vector<ClassFiles> bunchFilesByClasses(std::vector<File> files);
};

} // namespace Cda
