#ifndef AOC8B_H
#define AOC8B_H

#include <string_view>
#include <array>
#include <vector>
#include <set>

/*
iterate map
identify valid antennas (0-9, A-Z, a-z)
for each antenna, identify corresponding antennas and so antinodes
use distance between each antinode and traverse along line, within bounds of map
*/

namespace aoc8b
{
    bool validAntenna(char c)
    {
        return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
    }

    template <std::size_t N>
    bool validAntinode(int newRow, int newCol, const std::array<std::string_view, N>& lines)
    {
        return ((newRow >= 0 && newRow < lines.size()) && (newCol >= 0 && newCol < lines.at(0).size()));
    }

    template <std::size_t N>
    void getEveryAntinode(std::set<std::pair<size_t, size_t>>& antinodes, size_t row, size_t col, size_t otherRow, size_t otherCol, const std::array<std::string_view, N>& lines)
    {
        int rowDiff {static_cast<int>(row - otherRow)};
        int colDiff {static_cast<int>(col - otherCol)};
        int newRow {static_cast<int>(row)};
        int newCol {static_cast<int>(col)};

        while (validAntinode(newRow, newCol, lines))
        {
            antinodes.insert({newRow, newCol});
            newRow += rowDiff;
            newCol += colDiff;
        }

        while (validAntinode(newRow, newCol, lines))
        {
            antinodes.insert({newRow, newCol});
            newRow -= rowDiff;
            newCol -= colDiff;
        }
    }

    template <std::size_t N>
    void getAndAddAntinodes(std::set<std::pair<size_t, size_t>>& antinodes, size_t row, size_t col, const std::array<std::string_view, N>& lines)
    {
        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                if (i == row && j == col)
                    continue;
                if (lines.at(i).at(j) == lines.at(row).at(col))
                    getEveryAntinode(antinodes, row, col, i, j, lines);
            }
        }
    }

    template <std::size_t N>
    std::set<std::pair<size_t, size_t>> findAntinodes(const std::array<std::string_view, N>& lines)
    {
        std::set<std::pair<size_t, size_t>> antinodes {};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                if (validAntenna(lines.at(i).at(j)))
                    getAndAddAntinodes<N>(antinodes, i, j, lines);
            }
        }

        return antinodes;
    }

    template <std::size_t N>
    int parseAndCountAntinodes(std::array<std::string_view, N> lines)
    {
        std::set<std::pair<size_t, size_t>> allUniqueAntinodes {findAntinodes<N>(lines)};

        return allUniqueAntinodes.size();
    }
}

#endif // AOC8B_H
