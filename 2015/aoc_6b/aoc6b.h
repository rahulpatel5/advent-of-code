#ifndef AOC6B_H
#define AOC6B_H

#include "Matrix.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace aoc6b
{
    std::vector<std::string> parseInstruction(std::string_view line)
    {
        size_t firstComma {line.find(',')};
        size_t preFirstCommaSpace {line.find_last_of(' ', firstComma)};
        size_t postFirstCommaSpace {line.find_first_of(' ', firstComma)};
        size_t secondComma {line.find(',', firstComma + 1)};
        size_t preSecondCommaSpace {line.find_last_of(' ', secondComma)};

        std::string instruction {line.substr(0, preFirstCommaSpace)};
        std::string firstDigit {line.substr(preFirstCommaSpace + 1, firstComma - preFirstCommaSpace)};
        std::string secondDigit {line.substr(firstComma + 1, postFirstCommaSpace - firstComma)};
        std::string thirdDigit {line.substr(preSecondCommaSpace + 1, secondComma - preSecondCommaSpace)};
        std::string fourthDigit {line.substr(secondComma + 1)};

        return {instruction, firstDigit, secondDigit, thirdDigit, fourthDigit};
    }

    void chooseInstruction(Matrix& matrix, std::string_view method, std::size_t rowIndex, std::size_t columnStart, std::size_t columnEnd)
    {
        if (method == "turn on")
            matrix.addToMatrix(rowIndex, columnStart, columnEnd, 1);
        else if (method == "toggle")
            matrix.addToMatrix(rowIndex, columnStart, columnEnd, 2);
        else if (method == "turn off")
            matrix.reduceMatrix(rowIndex, columnStart, columnEnd);
        else
            std::cerr << "Unknown method encountered: " << method << '\n';
    }

    void useInstructionOnMatrix(Matrix& matrix, const std::vector<std::string>& instruction)
    {
        const std::size_t startRow {std::stoul(instruction[2])};
        const std::size_t endRow {std::stoul(instruction[4])};
        const std::size_t startCol {std::stoul(instruction[1])};
        const std::size_t endCol {std::stoul(instruction[3])};

        for (std::size_t row{startRow}; row <= endRow; ++row)
        {
            chooseInstruction(matrix, instruction[0], row, startCol, endCol);
        }
    }

    int runLightInstructions(const std::string& input, int lightRows, int lightColumns)
    {
        std::istringstream passage {input};
        std::string line{};

        Matrix lightMatrix{lightRows, lightColumns};

        while (std::getline(passage, line))
        {
            const std::vector<std::string> instruction {parseInstruction(line)};
            useInstructionOnMatrix(lightMatrix, instruction);
        }
        return countActiveLights(lightMatrix);
    }
}

#endif // AOC6B_H
