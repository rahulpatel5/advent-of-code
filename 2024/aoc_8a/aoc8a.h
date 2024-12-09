#ifndef AOC8A_H
#define AOC8A_H

#include <string_view>
#include <array>
#include <vector>
#include <set>

/*
iterate map
identify valid antennas (0-9, A-Z, a-z)
for each antenna, identify corresponding antennas and so antinodes
check that antinodes are valid (within confines of map)
count valid antinodes
*/

namespace aoc8a
{
    bool validAntenna(char c)
    {
        return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
    }

    template <std::size_t N>
    bool validAntinode(int rowDiff, int colDiff, size_t row, size_t col, const std::array<std::string_view, N>& lines)
    {
        return ((rowDiff + (int)row >= 0 && rowDiff + (int)row < lines.size()) && (colDiff + (int)col >= 0 && colDiff + (int)col < lines.at(0).size()));
    }

    template <std::size_t N>
    void getAndAddAntinodes(std::set<std::pair<size_t, size_t>>& antinodes, size_t row, size_t col, std::array<std::string_view, N> lines)
    {
        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines.at(i).size(); ++j)
            {
                if (i == row && j == col)
                    continue;
                if (lines.at(i).at(j) == lines.at(row).at(col))
                {
                    int rowDiff {static_cast<int>(row - i)};
                    int colDiff {static_cast<int>(col - j)};
                    if (validAntinode(rowDiff, colDiff, row, col, lines))
                        antinodes.insert({row + rowDiff, col + colDiff});
                }
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

#endif // AOC8A_H
