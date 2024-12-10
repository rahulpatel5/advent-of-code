#ifndef AOC2B_H
#define AOC2B_H

#include "../aoc_2a/aoc2a.h"
#include "../../shared/helper.h"

namespace aoc2b
{
    int getRibbon(int l, int w, int h)
    {
        int area{ l * w * h };

        int factor{ 2 }; // multiplication factor for perimeter
        int maxSide{ helper::maxThreeNumbers(l, w, h) };
        int perimeter{ (l + w + h - maxSide) * factor };

        return (area + perimeter);
    }

    int processLines(std::string& input)
    {
        std::istringstream passage{ input };
        std::string line;
        int area{0};

        while (std::getline(passage, line))
        {
            std::vector<int> numbers{ aoc2a::getNumbersFromSplit(line, 'x') };
            area += getRibbon(numbers[0], numbers[1], numbers[2]);
        }

        return area;
    }
}

#endif
