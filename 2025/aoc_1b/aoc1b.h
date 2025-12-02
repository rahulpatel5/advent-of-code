#ifndef AOC1B_H
#define AOC1B_H

#include <array>
#include <string_view>
#include <string>
#include <tuple>
#include <stdexcept>

/*
FOR each instruction:
    MOVE to new position using direction and turn amount
    IF position lands on or crosses 0:
        INCREMENT final count

RETURN final count
*/

namespace aoc1b
{
    using Password = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Position = long long;
    using Direction = char;
    using Turn = long long;
    using RotationInfo = std::tuple<Direction, Turn>;

    const int dialLimit { 99 };
    const int startPosition { 50 };
    const int targetPosition { 0 };

    RotationInfo getRotationInfo(const std::string& line)
    {
        return std::make_tuple(line[0], std::stoll(line.substr(1)));
    }

    void updatePositionAndTrackZeroes(Position& position, const RotationInfo& rotation, Password& zeroCount)
    {
        bool isStartAtZero { position == 0 };

        if (std::get<0>(rotation) == 'L')
            position -= std::get<1>(rotation);
        else if (std::get<0>(rotation) == 'R')
            position += std::get<1>(rotation);
        else
            throw std::invalid_argument("Got an unexpected value for the direction.\n");

        if (position == targetPosition)
            ++zeroCount;
        
        // handle situation where start at 0
        // then reduce by a value not divisible by 100
        // i.e. don't want to double-count reaching 0
        if (isStartAtZero && position < 0 && position % (dialLimit + 1) != 0)
            --zeroCount;

        while (position < 0 || position > dialLimit)
        {
            if (position < 0)
            {
                position += (dialLimit + 1);

                // handle situation where didn't start at 0 but end at 0
                if (!isStartAtZero & position == 0)
                    ++zeroCount;
            }
            else if (position > dialLimit)
            {
                position -= (dialLimit + 1);
            }
            ++zeroCount;
        }
    }

    template <std::size_t L>
    Password getPassword(const Lines<L>& lines)
    {
        Password zeroCount {0};

        Position currentPosition { startPosition };
        for (const std::string_view line : lines)
        {
            RotationInfo rotation { getRotationInfo(std::string(line)) };
            updatePositionAndTrackZeroes(currentPosition, rotation, zeroCount);
        }

        return zeroCount;
    }
}

#endif // AOC1B_H
