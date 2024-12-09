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

    // use template programming to ensure resolution at compile time
    // need this to create std::array (i.e. to define its fixed size)
    template <std::size_t length>
    constexpr int countUniqueLocations()
    {
        /*
        Number of journeys between 2 places is a binomial coefficient
        i.e. nCr = n! / (r! (n-r)!)
        We can therefore derive the number of unique locations
        i.e. we have the binomial coefficient and r = 2, and we want n
        nCr = n! / (2! (n-2)!) = n(n-1) / 2
        n**2 - n - 2nCr = 0
        Use quadratic solution to solve for n (we only want one solution)
        n = (1 + sqrt(1 + 4 * 2nCr)) / 2
        */
        constexpr double n {(1 + sqrt(1 + 8 * length)) / 2};
        static_assert(numberIsInteger(n) && "Number of unique locations should be an integer but it is not.");
        return static_cast<int>(n);
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

    template <typename T, auto locations>
    constexpr T countNonRepeatJourneys()
    {
        /*
        Total number of permutations are N! journeys (as above)
        However, a reversed journey is a duplicate
        e.g. A->B->C->D is the same as D->C->B->A
        Need to divide by 2 to remove duplicates, i.e. N! / 2 journeys
        */
        return factorial(locations) / 2;
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
