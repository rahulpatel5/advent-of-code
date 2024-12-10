#ifndef AOC10B_H
#define AOC10B_H

#include <string_view>
#include <array>
#include <vector>
#include <set>

/*
find each starting location ('0')
from each starting position, run through all valid paths
collect each path from that starting position
count all paths
sum ratings of each starting location
*/

namespace aoc10b
{
    using Paths = std::set<std::vector<std::pair<size_t, size_t>>>;

    template <std::size_t N>
    bool isValidPosition(size_t row, size_t col, const std::array<std::string_view, N>& lines, char height)
    {
        if ((row >= 0) && (row < lines.size()) && (col >= 0) && (col < lines.at(0).size()) && (lines.at(row).at(col) == height))
            return true;
        else
            return false;
    }

    template <std::size_t N>
    Paths recursiveTraverse(const Paths& paths, char height, const std::array<std::string_view, N>& lines)
    {
        Paths newPaths {};
        for (const std::vector<std::pair<size_t, size_t>>& path : paths)
        {
            size_t lastIndex {static_cast<size_t>(height - '0' - 1)};
            // check north
            if (isValidPosition(path.at(lastIndex).second - 1, path.at(lastIndex).first, lines, height))
            {
                std::vector<std::pair<size_t, size_t>> copyPath {path};
                copyPath.push_back({path.at(lastIndex).first, path.at(lastIndex).second - 1});
                newPaths.insert(copyPath);
            }
            // check east
            if (isValidPosition(path.at(lastIndex).second, path.at(lastIndex).first + 1, lines, height))
            {
                std::vector<std::pair<size_t, size_t>> copyPath {path};
                copyPath.push_back({path.at(lastIndex).first + 1, path.at(lastIndex).second});
                newPaths.insert(copyPath);
            }
            // check south
            if (isValidPosition(path.at(lastIndex).second + 1, path.at(lastIndex).first, lines, height))
            {
                std::vector<std::pair<size_t, size_t>> copyPath {path};
                copyPath.push_back({path.at(lastIndex).first, path.at(lastIndex).second + 1});
                newPaths.insert(copyPath);
            }
            // check west
            if (isValidPosition(path.at(lastIndex).second, path.at(lastIndex).first - 1, lines, height))
            {
                std::vector<std::pair<size_t, size_t>> copyPath {path};
                copyPath.push_back({path.at(lastIndex).first - 1, path.at(lastIndex).second});
                newPaths.insert(copyPath);
            }
        }

        if (height == '9')
            return newPaths;
        else
            return recursiveTraverse(newPaths, height + 1, lines);
    }

    template <std::size_t N>
    int countTrailheadRating(const std::array<std::string_view, N>& lines, size_t row, size_t col)
    {
        char height {'0'};
        Paths paths {{{std::make_pair(col, row)}}};
        Paths uniqueTrailheads {recursiveTraverse<N>(paths, height + 1, lines)};

        return uniqueTrailheads.size();
    }

    template <std::size_t N>
    int parseAndGetTotalRating(const std::array<std::string_view, N>& lines)
    {
        int score {0};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                if (lines.at(i).at(j) == '0')
                    score += countTrailheadRating<N>(lines, i, j);
            }
        }

        return score;
    }
}

#endif // AOC10B_H
