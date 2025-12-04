#ifndef AOC4B_H
#define AOC4B_H

#include <array>
#include <string_view>
#include <string>

/*
FOR each roll of paper:
    COUNT number of rolls of paper in adjacent tiles
    IF <4:
        REMOVE roll of paper
        ADD 1 to sum

REPEAT above steps until no more rolls of paper can be removed

RETURN sum
*/

namespace aoc4b
{
    using PaperInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    template <std::size_t N>
    using Map = std::array<std::string, N>;

    template <std::size_t L>
    Map<L> copyMap(const Lines<L>& lines)
    {
        Map<L> map {};
        for (size_t i{0}; i < L; ++i)
        {
            map[i] = std::string(lines.at(i));
        }
        return map;
    }

    template <std::size_t L>
    bool checkIfRollAtTile(size_t row, size_t col, const Map<L>& map)
    {
        return map.at(row).at(col) == '@';
    }

    template <std::size_t L>
    int countAdjacentRolls(size_t row, size_t col, const Map<L>& map)
    {
        int adjacent {0};
        // check above, if not on top edge
        if (row > 0)
        {
            // check to top left, if not on left edge
            if (col > 0 && checkIfRollAtTile<L>(row - 1, col - 1, map))
                ++adjacent;
            // check directly above
            if (checkIfRollAtTile<L>(row - 1, col, map))
                ++adjacent;
            // check to top right, if not on right edge
            if (col < map.at(row).size() - 1 && checkIfRollAtTile<L>(row - 1, col + 1, map))
                ++adjacent;
        }
        
        // check to left, if not on left edge
        if (col > 0)
        {
            if (checkIfRollAtTile<L>(row, col - 1, map))
                ++adjacent;
        }
        // check to right, if not on right edge
        if (col < map.at(row).size() - 1)
        {
            if (checkIfRollAtTile<L>(row, col + 1, map))
                ++adjacent;
        }
        
        // check below, if not on bottom edge
        if (row < map.size() - 1)
        {
            // check to bottom left, if not on left edge
            if (col > 0 && checkIfRollAtTile<L>(row + 1, col - 1, map))
                ++adjacent;
            // check directly below
            if (checkIfRollAtTile<L>(row + 1, col, map))
                ++adjacent;
            // check to bottom right, if not on right edge
            if (col < map.at(row).size() - 1 && checkIfRollAtTile<L>(row + 1, col + 1, map))
                ++adjacent;
        }
        return adjacent;
    }

    template <std::size_t L>
    void removePaperRolls(Map<L>& map, PaperInt& removed)
    {
        for (size_t row{0}; row < map.size(); ++row)
        {
            for (size_t col{0}; col < map.at(row).size(); ++col)
            {
                if (map.at(row).at(col) != '@')
                    continue;
                
                if (countAdjacentRolls(row, col, map) < 4)
                {
                    ++removed;
                    map[row][col] = '.';
                }
            }
        }
    }

    template <std::size_t L>
    PaperInt countPaperRolls(const Lines<L>& lines)
    {
        Map map { copyMap<L>(lines) };

        PaperInt removed {0};
        while (true)
        {
            PaperInt initialRemoved {removed};
            removePaperRolls(map, removed);

            // exit status is if there's no change in rolls removed
            if (removed == initialRemoved)
                break;
        }

        return removed;
    }
}

#endif // AOC4B_H
