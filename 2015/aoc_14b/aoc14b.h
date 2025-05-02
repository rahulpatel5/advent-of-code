#ifndef AOC14B_H
#define AOC14B_H

#include <array>
#include <string_view>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include <algorithm>

/*
re-use part 1 solution
instead of directly calculating distance, track each second
*/

namespace aoc14b
{
    using Points = long long;
    using Leaderboard = std::vector<Points>;

    using Reindeer = int;
    using Speed = int;
    using Time = int;
    using MoveTime = Time;
    using RestTime = Time;
    using RaceMap = std::map<Reindeer, std::tuple<Speed, MoveTime, RestTime, Time>>;
    using Distance = int;
    using Distances = std::vector<Distance>;
    using Reindeers = std::vector<Reindeer>;

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

            racers[reindeerIndex] = {speed, moveTime, restTime, moveTime + restTime};

            ++reindeerIndex;
        }

        return reindeerIndex;
    }

    Points maxPoints(int numberRacers, const RaceMap& racers, int raceTime)
    {
        Leaderboard scores {};
        Distances distances {};
        for (int _i{0}; _i < numberRacers; ++_i)
        {
            scores.push_back(0);
            distances.push_back(0);
        }

        Points maxDist {0};
        Reindeers pointWinners {};
        for (int t{0}; t < raceTime; ++t)
        {
            for (int n{0}; n < numberRacers; ++n)
            {
                if ((t % std::get<3>(racers.at(n))) < std::get<1>(racers.at(n)))
                {
                    distances[n] += std::get<0>(racers.at(n));
                }

                if (distances.at(n) > maxDist)
                {
                    maxDist = distances.at(n);
                    pointWinners = {n};
                }
                else if (distances.at(n) == maxDist && std::find(pointWinners.begin(), pointWinners.end(), n) == pointWinners.end())
                {
                    pointWinners.push_back(n);
                }
            }
            for (Reindeer r : pointWinners)
                scores[r] += 1;
        }

        Points max {0};
        for (Points p : scores)
        {
            if (p > max)
                max = p;
        }
        return max;
    }

    template <std::size_t N, int T>
    Points getWinningPoints(const std::array<std::string_view, N>& lines)
    {
        RaceMap racers {};
        int numberRacers {processInput<N>(lines, racers)};

        return maxPoints(numberRacers, racers, T);
    }
}

#endif // AOC14B_H
