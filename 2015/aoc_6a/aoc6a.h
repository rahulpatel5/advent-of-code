#ifndef AOC6A_H
#define AOC6A_H

#include "BitMatrix.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace aoc6a
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

    std::bitset<1000> setUpBitMask(const std::string& start, const std::string& end)
    {
        std::bitset<1000> bitMask{};
        bitMask.set();

        const int startDigit {std::stoi(start)};
        const int endDigit {std::stoi(end)};
        bitMask <<= (999 - (endDigit - startDigit));
        bitMask >>= startDigit;

        return bitMask;
    }

    void chooseInstruction(BitMatrix& matrix, std::string_view method, std::size_t rowIndex, std::bitset<1000> bitMask)
    {
        if (method == "turn on")
            matrix.setRowSET(rowIndex, bitMask);
        else if (method == "toggle")
            matrix.setRowXOR(rowIndex, bitMask);
        else if (method == "turn off")
            matrix.setRowNOT(rowIndex, bitMask);
        else
            std::cerr << "Unknown method encountered: " << method << '\n';
    }

    void useInstructionOnMatrix(BitMatrix& matrix, const std::vector<std::string>& instruction)
    {
        std::bitset<1000> bitMask{setUpBitMask(instruction[1], instruction[3])};

        const std::size_t startRow {std::stoul(instruction[2])};
        const std::size_t endRow {std::stoul(instruction[4])};
        for (std::size_t row{startRow}; row <= endRow; ++row)
        {
            chooseInstruction(matrix, instruction[0], row, bitMask);
        }
    }

    int runLightInstructions(const std::string& input, int lightRows)
    {
        std::istringstream passage {input};
        std::string line{};

        BitMatrix lightMatrix{lightRows};

        while (std::getline(passage, line))
        {
            const std::vector<std::string> instruction {parseInstruction(line)};
            useInstructionOnMatrix(lightMatrix, instruction);
        }
        return countActiveLights(lightMatrix);
    }
}

#endif // AOC6A_H
