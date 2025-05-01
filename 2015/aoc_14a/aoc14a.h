#ifndef AOC14A_H
#define AOC14A_H

#include <array>
#include <string_view>
#include <string>
#include <map>
#include <tuple>

namespace aoc14a
{
    using DistInt = long long;

    using Reindeer = int;
    using Speed = int;
    using Time = int;
    using MoveTime = Time;
    using RestTime = Time;
    using RaceMap = std::map<Reindeer, std::tuple<Speed, MoveTime, RestTime>>;
    using RepeatPeriods = long long;

    template <std::size_t N>
    int processInput(const std::array<std::string_view, N>& lines, RaceMap& racers)
    {
        Reindeer reindeerIndex {0};

        for (std::string_view row : lines)
        {
            std::string line {std::string(row)};

            size_t fly {line.find("fly")};
            size_t flySpace {line.find(' ', fly)};
            size_t speedSpace {line.find(' ', flySpace + 1)};
            Speed speed {std::stoi(line.substr(flySpace + 1, speedSpace - (flySpace + 1)))};

            size_t firstFor {line.find("for", speedSpace)};
            size_t for1Space {line.find(' ', firstFor)};
            size_t moveSpace {line.find(' ', for1Space + 1)};
            MoveTime moveTime {std::stoi(line.substr(for1Space + 1, moveSpace - (for1Space + 1)))};

            size_t secondFor {line.find("for", moveSpace)};
            size_t for2Space {line.find(' ', secondFor)};
            size_t restSpace {line.find(' ', for2Space + 1)};
            RestTime restTime {std::stoi(line.substr(for2Space + 1, restSpace - (for2Space + 1)))};

            racers[reindeerIndex] = {speed, moveTime, restTime};

            ++reindeerIndex;
        }

        return reindeerIndex;
    }

    DistInt maxDistance(int numberRacers, const RaceMap& racers, int raceTime)
    {
        DistInt maxDist {0};

        for (int n{0}; n < numberRacers; ++n)
        {
            Time period {std::get<1>(racers.at(n)) + std::get<2>(racers.at(n))};
            RepeatPeriods repeats {raceTime / period};
            // extra movement, depending how much time left
            Time spareTime {raceTime % period};
            spareTime = (spareTime < std::get<1>(racers.at(n)) ? spareTime : std::get<1>(racers.at(n)));

            DistInt raceDist {(repeats * std::get<1>(racers.at(n)) + spareTime) * std::get<0>(racers.at(n))};

            if (raceDist > maxDist)
                maxDist = raceDist;
        }

        return maxDist;
    }

    template <std::size_t N, int T>
    DistInt getWinningDistance(const std::array<std::string_view, N>& lines)
    {
        RaceMap racers {};
        int numberRacers {processInput<N>(lines, racers)};

        return maxDistance(numberRacers, racers, T);
    }
}

#endif // AOC14A_H
