#ifndef AOC14B_H
#define AOC14B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <stdexcept>
#include <map>
#include <algorithm>

/*
algorithm using initial positions and velocities to find tree
    - for each second, iterate over all robots
    - collect number of each robots in each column
    - if column count meets threshold, count consecutive robots
    - if consecutive count meets threshold, return solution

this only gets the solution time down to c2s
to go further, need to reduce number of iterations
max size of columns in each loop is much higher on certain loops
the solution fits into this pattern
i.e. we can skip many loops by only iterating over select loops
this may make the solution less generalised
*/

namespace aoc14b
{
    using Robot = std::pair<std::pair<int, int>, std::pair<int, int>>;
    using Robots = std::vector<Robot>;
    using VectorPairs = std::vector<std::pair<int, int>>;

    template <std::size_t N>
    void getStartingPositionsAndVelocities(const std::array<std::string_view, N>& lines, VectorPairs& positions, VectorPairs& velocities)
    {
        for (std::string_view line : lines)
        {
            size_t firstEquals {line.find('=')};
            size_t firstComma {line.find(',')};
            size_t space {line.find(' ')};
            int xPos {std::stoi(std::string(line.substr(firstEquals + 1, firstComma - (firstEquals + 1))))};
            int yPos {std::stoi(std::string(line.substr(firstComma + 1, space - (firstComma + 1))))};

            size_t secondEquals {line.find('=', space + 1)};
            size_t secondComma {line.find(',', space + 1)};
            int xVel {std::stoi(std::string(line.substr(secondEquals + 1, secondComma - (secondEquals + 1))))};
            int yVel {std::stoi(std::string(line.substr(secondComma + 1)))};

            positions.push_back({xPos, yPos});
            velocities.push_back({xVel, yVel});
        }
        assert ((positions.size() == velocities.size()) && "There are not the same number of starting positions and velocities.\n");
    }

    // int consecRows(const std::set<int>& rows)
    int consecRows(std::vector<int>& rows)
    {
        std::sort(rows.begin(), rows.end());
        int max {};
        int count {};
        for (size_t i{0}; i < rows.size(); ++i)
        {
            if (i > 0 && rows.at(i) == rows.at(i - 1) + 1)
            {
                ++count;
                if (count > max)
                    max = count;
            }
            else
            {
                count = 1;
            }
        }
        return max;
    }

    int getTimeToTree(VectorPairs positions, const VectorPairs& velocities, std::pair<int, int> map)
    {
        int time {0};
        int threshold {map.second / 4};

        // speed up code by identifying pattern & skipping loops
        int approxAvg {};
        int tolerance {2};
        int loopPattern {0};
        int prevLoop {};

        while (time < 10'000)
        {
            std::map<int, std::vector<int>> columnCount {};

            if (loopPattern > 0 && time != prevLoop + loopPattern)
            {
                time = prevLoop + loopPattern;
            }

            int max {};
            for (size_t i{0}; i < positions.size(); ++i)
            {
                int col {(positions.at(i).first + velocities.at(i).first * time) % map.first};
                
                columnCount[col].push_back((positions.at(i).second + velocities.at(i).second * time) % map.second);

                if (columnCount.at(col).size() >= threshold && consecRows(columnCount.at(col)) >= threshold)
                {
                    return time;
                }

                if (columnCount.at(col).size() > max)
                    max = columnCount.at(col).size();
            }

            if (time == 0)
                approxAvg = max;
            else if (max > approxAvg * tolerance)
            {
                if (loopPattern == 0 && prevLoop != 0)
                    loopPattern = time - prevLoop;
                prevLoop = time;
            }

            ++time;
        }
        throw std::out_of_range("Didn't find a valid solution.\n");
    }

    template <std::size_t N>
    int parseAndGetTreeTime(const std::array<std::string_view, N>& lines, std::pair<int, int> map)
    {
        VectorPairs positions {};
        VectorPairs velocities {};
        getStartingPositionsAndVelocities<N>(lines, positions, velocities);

        // make all velocities positive
        for (auto it{velocities.begin()}; it != velocities.end(); ++it)
        {
            if (it->first < 0)
                it->first += map.first;
            if (it->second < 0)
                it->second += map.second;
        }

        return getTimeToTree(positions, velocities, map);
    }
}

#endif // AOC14B_H
