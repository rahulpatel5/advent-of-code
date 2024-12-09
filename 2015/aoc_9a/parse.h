#ifndef PARSE_H
#define PARSE_H

#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <math.h>

namespace parse
{
    constexpr bool numberIsInteger(auto n)
    {
        return floorf(n) == n;
    }

    constexpr auto factorial(auto n)
    {
        auto total {1};
        for (auto i{n}; i > 1; --i)
        {
            total *= i;
        }
        return total;
    }

    constexpr std::vector<std::string> readLocationData(std::string_view line)
    {
        size_t firstSpace {line.find_first_of(' ')};
        size_t secondSpace {line.find_first_of(' ', firstSpace + 1)};
        size_t thirdSpace {line.find_first_of(' ', secondSpace + 1)};
        size_t lastSpace {line.find_last_of(' ')};

        std::string firstLocation {line.substr(0, firstSpace)};
        std::string secondLocation {line.substr(secondSpace + 1, (thirdSpace - (secondSpace + 1)))};
        std::string distance {line.substr(lastSpace + 1)};

        return {firstLocation, secondLocation, distance};
    }

    constexpr void addUniqueLocations(const std::vector<std::string>& locations, std::set<std::string>& uniqueLocations)
    {
        // don't want the last string in the array (the distance)
        for (auto i{0}; i < locations.size() - 1; ++i)
        {
            uniqueLocations.insert(locations[i]);
        }
    }
}

#endif // PARSE_H
