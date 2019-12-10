#pragma once

#include <string>
#include <vector>

namespace Cda
{

namespace Utils
{

    namespace Strings
    {
        std::wstring trim(std::wstring str);

        std::string trim(std::string str);

        bool endsWith(const std::wstring& str, const std::wstring& token);

        bool endsWith(const std::string& str, const std::string& token);

        bool startsWith(const std::wstring& str, const std::wstring& token);

        bool startsWith(const std::string& str, const std::string& token);

        std::wstring convert(const std::string& str);

        std::string convert(const std::wstring& str);
    }

} // Utils

} // Cda
