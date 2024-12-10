#ifndef AOC2A_H
#define AOC2A_H

#include "../../shared/helper.h"
#include <sstream>
#include <vector>

namespace aoc2a
{
    std::vector<int> getNumbersFromSplit(std::string& line, char separator)
    {
        std::string::size_type firstX{ line.find(separator) };
        int firstNumber{ std::stoi( line.substr(0, firstX) ) };
        std::string::size_type secondX{ line.substr(firstX + 1).find(separator) };
        int secondNumber{ std::stoi( line.substr(firstX + 1, secondX) ) };
        int thirdNumber{ std::stoi( line.substr(firstX + secondX + 2) ) };

        return { firstNumber, secondNumber, thirdNumber };
    }

    int getWrappingPaper(int l, int w, int h)
    {
        int factor{ 2 }; // multiplication factor for each area
        int area1{ factor * l * w };
        int area2{ factor * w * h };
        int area3{ factor * h * l };

        return (area1 + area2 + area3 + helper::minThreeNumbers(area1, area2, area3) / factor);
    }

    int processLines(std::string& input)
    {
        std::istringstream passage{ input };
        std::string line;
        int area{0};

        while (std::getline(passage, line))
        {
            std::vector<int> numbers{ getNumbersFromSplit(line, 'x') };
            area += getWrappingPaper(numbers[0], numbers[1], numbers[2]);
        }

        return area;
    }
}

#endif
