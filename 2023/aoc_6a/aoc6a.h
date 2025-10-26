#ifndef AOC6A_H
#define AOC6A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>

/*
FOR each race:
    FOR each time:
        IF speed and time would beath the record:
            COUNT + 1
    KEEP total count for each race

RETURN counts multiplied together
*/

namespace aoc6a
{
    using TimeInt = long long;
    template <std::size_t L>
    using Lines = std::array<std::string_view, L>;

    using Times = std::vector<TimeInt>;
    using Distance = long long;
    using Distances = std::vector<Distance>;

    template <std::size_t N>
    void getRaceInfo(Times& times, Distances& distances, const Lines<N>& lines)
    {
        for (std::string_view line : lines)
        {
            std::string row { std::string(line) };
            size_t index { row.find(':') + 1 };

            std::string info { row.substr(0, index - 1) };

            while (index != std::string::npos)
            {
                size_t numberStart {row.find_first_not_of(' ', index)};
                size_t numberEnd {row.find_first_of(' ', numberStart)};

                std::string digitStr { row.substr(numberStart, numberEnd - numberStart) };
                if (info == "Time")
                {
                    times.push_back(std::stoll(digitStr));
                }
                else if (info == "Distance")
                {
                    distances.push_back(std::stoll(digitStr));
                }

                index = numberEnd;
            }
        }
    }

    TimeInt countWinsForRace(TimeInt time, Distance distance)
    {
        TimeInt successCount {0};

        for (long long index{0}; index < time; ++index)
        {
            long long speed { index };
            long long remainingTime { time - index };
            if (speed * remainingTime > distance)
                ++successCount;
        }
        
        return successCount;
    }

    template <std::size_t N>
    TimeInt getTravelTime(const Lines<N>& lines)
    {
        Times times {};
        Distances distances {};
        getRaceInfo(times, distances, lines);
        assert(times.size() == distances.size() && "Number of time elements is not the same as the number of distance elements.\n");

        TimeInt product {1};
        for (size_t race{0}; race < times.size(); ++race)
        {
            product *= countWinsForRace(times.at(race), distances.at(race));
        }

        return product;
    }
}

#endif // AOC6A_H
