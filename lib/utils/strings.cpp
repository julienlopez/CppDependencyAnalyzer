#include "strings.hpp"

#include <cctype>

template <class STRING> bool endsWith(const STRING& str, const STRING& token)
{
    if(token.size() > str.size()) return false;
    return std::equal(token.rbegin(), token.rend(), str.rbegin());
}

template <class STRING> STRING trim(STRING str)
{
    while(!str.empty() && std::isspace(str.front()))
        str.erase(str.begin());
    while(!str.empty() && std::isspace(str.back()))
        str.erase(--str.end());
    return str;
}

namespace Cda
{

namespace Utils
{

    std::wstring Strings::trim(std::wstring str)
    {
        return ::trim<std::wstring>(str);
    }

    std::string Strings::trim(std::string str)
    {
        return ::trim<std::string>(str);
    }

    bool Strings::endsWith(const std::wstring& str, const std::wstring& token)
    {
        return ::endsWith<std::wstring>(str, token);
    }

    bool Strings::endsWith(const std::string& str, const std::string& token)
    {
        return ::endsWith<std::string>(str, token);
    }

    std::wstring Strings::convert(const std::string& str)
    {
        return {str.begin(), str.end()};
    }

    std::string Strings::convert(const std::wstring& str)
    {
        return {str.begin(), str.end()};
    }

} // Utils

} // Cda
