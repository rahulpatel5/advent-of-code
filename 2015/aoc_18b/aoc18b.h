#ifndef AOC18B_H
#define AOC18B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

/*
* SWITCH on lights in corners PERMANENTLY

IF on light surrounded by 2 or 3 on lights
    THEN collect as a light to switch off
IF off light surrounded by 3 on lights
    THEN collect as a light to switch on

SWITCH on/off lights based on above
REPEAT for number of steps
COUNT final number of on lights

* at start of line shows changes to the part 1 solution
*/

namespace aoc18b
{
    using LightInt = int;
    constexpr char ON_LIGHT {'#'};
    constexpr char OFF_LIGHT {'.'};

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;
    template <std::size_t N>
    using Map = std::array<std::string, N>;

    // the "x co-ordinate"
    using Col = size_t;
    // the "y co-ordinate"
    using Row = size_t;
    using Light = std::pair<Col, Row>;
    using OnLights = std::vector<Light>;
    using OffLights = std::vector<Light>;
    using Positions = std::vector<Light>;

    template <std::size_t N>
    Map<N> convertToMap(const Lines<N>& lines)
    {
        Map<N> map {};
        for (size_t i{0}; i < lines.size(); ++i)
        {
            map[i] = std::string(lines.at(i));
        }
        return map;
    }

    template <std::size_t N>
    void switchOnCorners(Map<N>& map)
    {
        // north west corner
        map[0][0] = ON_LIGHT;
        // north east corner
        map[0][map[0].size() - 1] = ON_LIGHT;
        // south east corner
        map[N - 1][map[N - 1].size()] = ON_LIGHT;
        //south west corner
        map[N - 1][0] = ON_LIGHT;
    }

    template <std::size_t N>
    bool isInCorner(const Map<N>& map, size_t col, size_t row)
    {
        return (
            // north west
            (row == 0 && col == 0)
            // north east
            || (row == 0 && col == map.at(0).size() - 1)
            // south east
            || (row == N - 1 && col == map.at(N - 1).size() - 1)
            // south west
            || (row == N - 1 && col == 0)
        );
    }

    template <std::size_t N>
    bool isLightOn(const Map<N>& map, size_t col, size_t row)
    {
        return map[row][col] == ON_LIGHT;
    }

    template <std::size_t N>
    bool isOutOfBounds(const Map<N>& map, size_t col, size_t row)
    {
        return !(col >= 0 && col < map.at(0).size() && row >= 0 && row < N);
    }

    template <std::size_t N>
    LightInt countAdjacentOnLights(const Map<N>& map, size_t col, size_t row)
    {
        LightInt count {0};
        // east
        count += (isOutOfBounds<N>(map, col + 1, row)) ? 0 : isLightOn<N>(map, col + 1, row);
        // south east
        count += (isOutOfBounds<N>(map, col + 1, row + 1)) ? 0 : isLightOn<N>(map, col + 1, row + 1);
        // south
        count += (isOutOfBounds<N>(map, col, row + 1)) ? 0 : isLightOn<N>(map, col, row + 1);
        // south west
        count += (isOutOfBounds<N>(map, col - 1, row + 1)) ? 0 : isLightOn<N>(map, col - 1, row + 1);
        // west
        count += (isOutOfBounds<N>(map, col - 1, row)) ? 0 : isLightOn<N>(map, col - 1, row);
        // north west
        count += (isOutOfBounds<N>(map, col - 1, row - 1)) ? 0 : isLightOn<N>(map, col - 1, row - 1);
        // north
        count += (isOutOfBounds<N>(map, col, row - 1)) ? 0 : isLightOn<N>(map, col, row - 1);
        // north east
        count += (isOutOfBounds<N>(map, col + 1, row - 1)) ? 0 : isLightOn<N>(map, col + 1, row - 1);

        return count;
    }

    template <std::size_t N>
    void checkLights(const Map<N>& map, OnLights& onLights, OffLights& offLights)
    {
        for (size_t y{0}; y < N; ++y)
        {
            for (size_t x{0}; x < map.at(y).size(); ++x)
            {
                LightInt lightCount {countAdjacentOnLights<N>(map, x, y)};

                if (isLightOn<N>(map, x, y) && (lightCount < 2 || lightCount > 3) && !isInCorner<N>(map, x, y))
                {
                    offLights.push_back({x, y});
                }
                else if (!isLightOn<N>(map, x, y) && lightCount == 3)
                {
                    onLights.push_back({x, y});
                }
            }
        }
    }

    template <std::size_t N>
    void switchLights(Map<N>& map, const OnLights& onLights, const OffLights& offLights)
    {
        for (const Light& lt : onLights)
        {
            map[lt.second][lt.first] = ON_LIGHT;
        }
        for (const Light& lt : offLights)
        {
            map[lt.second][lt.first] = OFF_LIGHT;
        }
    }

    template <std::size_t N>
    LightInt countLights(const Map<N>& map)
    {
        return std::accumulate(map.begin(), map.end(), 0, [](int sum, const std::string& s) {
            return sum + std::count(s.begin(), s.end(), ON_LIGHT);
        });
    }

    template <std::size_t L, int steps>
    LightInt countOnLights(const Lines<L>& lines)
    {
        Map<L> map {convertToMap<L>(lines)};
        switchOnCorners<L>(map);

        for (int i{0}; i < steps; ++i)
        {
            OnLights onLights {};
            OffLights offLights {};
            checkLights<L>(map, onLights, offLights);

            switchLights<L>(map, onLights, offLights);
        }

        return countLights<L>(map);
    }
}

#endif // AOC18B_H
