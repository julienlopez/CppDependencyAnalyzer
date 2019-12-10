#include "strings.hpp"

#include <cctype>
#include <sstream>

namespace
{

template <class STRING> bool startsWith(const STRING& str, const STRING& token)
{
    if(token.size() > str.size()) return false;
    return std::equal(token.begin(), token.end(), str.begin());
}

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
template <class STRING> auto split(const STRING& str, typename STRING::value_type separator)
{
    std::vector<STRING> res;
    STRING line;
    std::basic_istringstream<typename STRING::value_type> stream(str);
    while(std::getline(stream, line, separator))
        res.push_back(line);
    return res;
}

} // namespace

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

    bool Strings::startsWith(const std::wstring& str, const std::wstring& token)
    {
        return ::startsWith<std::wstring>(str, token);
    }

    bool Strings::startsWith(const std::string& str, const std::string& token)
    {
        return ::startsWith<std::string>(str, token);
    }

    std::wstring Strings::convert(const std::string& str)
    {
        return {str.begin(), str.end()};
    }

    std::string Strings::convert(const std::wstring& str)
    {
        return {str.begin(), str.end()};
    }

    std::vector<std::string> Strings::split(const std::string& str, char separator)
    {
        return ::split<std::string>(str, separator);
    }

    std::vector<std::wstring> Strings::split(const std::wstring& str, wchar_t separator)
    {
        return ::split<std::wstring>(str, separator);
    }

} // namespace Utils

} // namespace Cda
