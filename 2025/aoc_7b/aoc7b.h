#ifndef AOC7B_H
#define AOC7B_H

#include <array>
#include <string_view>
#include <string>
#include <map>
#include <stdexcept>
#include <ranges>

/*
FIND start column position

FOR each beam:
    IF splitter is in the way:
        ADD two new beams and stop the existing one

RETURN the total number of beams at the end

similar to part 1 solution, but don't merge beams?

checked an assumption in part 1 that we won't check again
*/

namespace aoc7b
{
    using SplitInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Index = std::size_t;
    // position 1: column index, position 2: row index
    using StartPosition = std::pair<Index, Index>;
    using BeamMap = std::map<Index, SplitInt>;

    template <std::size_t L>
    StartPosition getStartPosition(const Lines<L>& lines)
    {
        for (size_t row_index{0}; row_index < lines.size(); ++row_index)
        {
            if (lines.at(row_index).find('S') == std::string_view::npos)
                continue;
            
            std::string row { std::string(lines.at(row_index)) };
            Index col { row.find('S') };
            return std::make_pair(col, row_index);
        }
        throw std::out_of_range("Did not find a starting position.\n");
    }

    SplitInt countBeams(const BeamMap& map)
    {
        SplitInt count {0};
        for (const auto v : map | std::ranges::views::values)
        {
            count += v;
        }
        return count;
    }

    template <std::size_t L>
    SplitInt countEverySplit(const Lines<L>& lines)
    {
        StartPosition start { getStartPosition<L>(lines) };

        BeamMap beamMap {};
        beamMap[start.first]++;
        for (size_t row_index{start.second + 1}; row_index < lines.size(); ++row_index)
        {
            if (lines.at(row_index).find('^') == std::string_view::npos)
                continue;

            std::string row { std::string(lines.at(row_index)) };
            Index pos {0};
            while (pos != std::string::npos)
            {
                pos = row.find('^', pos);
                if (pos != std::string::npos)
                {
                    if (beamMap.find(pos) != beamMap.end())
                    {
                        beamMap[pos - 1] += beamMap[pos];
                        beamMap[pos + 1] += beamMap[pos];
                        // remove the beams on the current column
                        // the assumption checked in part 1 means we don't
                        // need to worry about newly split beams in this col
                        beamMap[pos] = 0;
                    }
                    ++pos;
                }
            }
        }

        return countBeams(beamMap);
    }
}

#endif // AOC7B_H
