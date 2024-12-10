#ifndef AOC10A_H
#define AOC10A_H

#include <string_view>
#include <array>
#include <vector>
#include <set>

/*
find each starting location ('0')
from each starting position, run through all valid paths
know that each point must be the same value (e.g. '1' -> '2' -> '3')
so use recursive function to collect each relevant position (N, E, W, S)
count number of unique '9's at end for the score for that starting location
sum scores of each starting location
*/

namespace aoc10a
{
    template <std::size_t N>
    bool isValidPosition(size_t row, size_t col, const std::array<std::string_view, N>& lines, char height)
    {
        if ((row >= 0) && (row < lines.size()) && (col >= 0) && (col < lines.at(0).size()) && (lines.at(row).at(col) == height))
            return true;
        else
            return false;
    }

    template <std::size_t N>
    std::set<std::pair<size_t, size_t>> recursiveTraverse(const std::set<std::pair<size_t, size_t>>& positions, char height, const std::array<std::string_view, N>& lines)
    {
        std::set<std::pair<size_t, size_t>> newPositions {};
        for (std::pair<size_t, size_t> position : positions)
        {
            // check north
            if (isValidPosition(position.second - 1, position.first, lines, height))
                newPositions.insert({position.first, position.second - 1});
            // check east
            if (isValidPosition(position.second, position.first + 1, lines, height))
                newPositions.insert({position.first + 1, position.second});
            // check south
            if (isValidPosition(position.second + 1, position.first, lines, height))
                newPositions.insert({position.first, position.second + 1});
            // check west
            if (isValidPosition(position.second, position.first - 1, lines, height))
                newPositions.insert({position.first - 1, position.second});
        }

        if (height == '9')
            return newPositions;
        else
            return recursiveTraverse(newPositions, height + 1, lines);
    }

    template <std::size_t N>
    int countTrailheadScore(const std::array<std::string_view, N>& lines, size_t row, size_t col)
    {
        char height {'0'};
        std::set<std::pair<size_t, size_t>> positions {{std::make_pair(col, row)}};
        std::set<std::pair<size_t, size_t>> uniqueTrailheads {recursiveTraverse<N>(positions, height + 1, lines)};

        return uniqueTrailheads.size();
    }

    template <std::size_t N>
    int parseAndGetTotalScore(const std::array<std::string_view, N>& lines)
    {
        int score {0};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                if (lines.at(i).at(j) == '0')
                    score += countTrailheadScore<N>(lines, i, j);
            }
        }

        return score;
    }
}

#endif // AOC10A_H
