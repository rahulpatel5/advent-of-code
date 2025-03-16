#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string_view>
#include <array>
#include <algorithm>

namespace StringUtils
{
    constexpr std::size_t length(std::string_view input)
    {
        return std::count(input.begin(), input.end(), '\n');
    }

    // gets the width of the first line
    constexpr std::size_t width(std::string_view input)
    {
        std::size_t endLine {input.find('\n')};
        return input.substr(0, endLine).size();
    }

    template <std::size_t N>
    constexpr std::array<std::string_view, N> split(std::string_view input)
    {
        size_t start {0};
        size_t end {0};
        size_t index {0};
        std::array<std::string_view, N> lines;

        while ((end = input.find('\n', start)) != std::string_view::npos)
        {
            lines[index++] = input.substr(start, end - start);
            start = end + 1;
        }

        return lines;
    }
}

#endif // STRINGUTILS_H
