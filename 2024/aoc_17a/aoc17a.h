#ifndef AOC17A_H
#define AOC17A_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>

/*
read in registers and program
set up opcodes and logic for instructions
where outputting, store output in a vector
when program halts, combine vector into a string
return final string
*/

namespace aoc17a
{
    template <std::size_t N>
    std::map<char, int> readRegisters(const std::array<std::string_view, N>& lines)
    {
        std::map<char, int> registers {};
        for (std::string_view line : lines)
        {
            if (line.find("Register ") == std::string_view::npos)
                continue;
            size_t colon {line.find(':')};
            int num {std::stoi(std::string(line.substr(colon + 2)))};
            size_t firstSpace {line.find(' ')};
            char reg {line.data()[firstSpace + 1]};
            registers[reg] = num;
        }
        return registers;
    }

    template <std::size_t N>
    std::vector<int> readProgram(const std::array<std::string_view, N>& lines)
    {
        std::vector<int> program {};

        std::string_view line {};
        for (std::string_view row : lines)
        {
            if (row.find("Program:") != std::string_view::npos)
                line = row;
        }

        size_t space {line.find(' ')};
        int num0 {line.data()[space + 1] - '0'};
        program.push_back(num0);

        size_t comma {line.find(',')};
        while (comma != std::string_view::npos)
        {
            int num {line.data()[comma + 1] - '0'};
            program.push_back(num);
            // need to move previous iteration of comma from previous match
            comma = line.find(',', ++comma);
        }
        return program;
    }

    int getComboOperand(int pointer, const std::vector<int>& program, const std::map<char, int>& registers)
    {
        int literalOperand {program[pointer + 1]};
        if (literalOperand < 4)
            return literalOperand;
        else if (literalOperand < 7)
            return registers.at('A' + (literalOperand - 4));
        else if (literalOperand == 7)
            throw std::invalid_argument("Invalid program: operand 7 reached.\n");
        else
            throw std::invalid_argument("Received unexpected operand.\n");
    }

    std::vector<int> runProgram(std::map<char, int>& registers, const std::vector<int>& program)
    {
        std::vector<int> output {};
        size_t pointer {0};
        // think it makes more sense to keep all this together
        // if this were a longer project, might be neater to send to functions
        while (pointer < program.size())
        {
            int literalOperand {program[pointer + 1]};
            int comboOperand {getComboOperand(pointer, program, registers)};
            // opcode 0 (adv)
            if (program[pointer] == 0)
                registers['A'] /= std::pow(2, comboOperand);
            // opcode 1 (bxl)
            else if (program[pointer] == 1)
                registers['B'] ^= literalOperand;
            // opcode 2 (bst)
            else if (program[pointer] == 2)
                registers['B'] = comboOperand % 8;
            // opcode 3 (jnz)
            else if (program[pointer] == 3)
            {
                if (registers['A'] == 0)
                    {/* deliberately empty */}
                else
                {
                    pointer = literalOperand;
                    continue;
                }
            }
            // opcode 4 (bxc)
            else if (program[pointer] == 4)
                registers['B'] ^= registers['C'];
            // opcode 5 (out)
            else if (program[pointer] == 5)
                output.push_back(comboOperand % 8);
            // opcode 6 (bdv)
            else if (program[pointer] == 6)
                registers['B'] = registers['A'] / std::pow(2, comboOperand);
            // opcode 7 (cdv)
            else if (program[pointer] == 7)
                registers['C'] = registers['A'] / std::pow(2, comboOperand);

            // move pointer
            pointer += 2;
        }
        return output;
    }

    std::string getFinalOutput(const std::vector<int>& output)
    {
        std::string final {std::to_string(output[0])};
        for (size_t i{1}; i < output.size(); ++i)
        {
            final.append(",");
            final.append(std::to_string(output[i]));
        }
        return final;
    }

    template <std::size_t N>
    std::string_view parseAndGetOutput(const std::array<std::string_view, N> lines)
    {
        //
        std::map<char, int> registers {readRegisters<N>(lines)};
        std::vector<int> program {readProgram<N>(lines)};
        std::vector<int> output {runProgram(registers, program)};
        return getFinalOutput(output);
    }
}

#endif // AOC17A_H
