#ifndef AOC23A_H
#define AOC23A_H

#include <array>
#include <string_view>
#include <string>
#include <stdexcept>

/*
IF instruction defined
    THEN carry out instruction (e.g. update register)
    MOVE to next appropriate instruction
ELSE
    STOP program

RETURN final value for register b
*/

namespace aoc23a
{
    using Register = unsigned long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Instruction = std::string_view;
    using Index = std::size_t;

    void jump(const std::string& instruct, Index& index, size_t space)
    {
        std::string sign {instruct.substr(space + 1, 1)};
        int jump {std::stoi(instruct.substr(space + 2))};
        if (sign == "+")
            index += jump;
        else if (sign == "-")
            index -= jump;
        else
            throw std::invalid_argument("Given an unexpected sign when jumping.\n");
    }

    std::string findRegister(const std::string& instruct)
    {
        size_t space {instruct.find(' ')};
        size_t comma {instruct.find(',')};
        return instruct.substr(space + 1, comma - (space + 1));
    }

    void processInstruction(Instruction instruction, Index& index, Register& registerA, Register& registerB)
    {
        std::string instruc {std::string(instruction)};
        size_t lastSpace {instruc.find_last_of(' ')};
        if (instruction.find("hlf") != std::string_view::npos)
        {
            if (instruc.substr(lastSpace + 1) == "a")
                registerA /= 2;
            else if (instruc.substr(lastSpace + 1) == "b")
                registerB /= 2;
            else
                throw std::invalid_argument("Given an unexpected register.\n");
        }
        else if (instruction.find("tpl") != std::string_view::npos)
        {
            if (instruc.substr(lastSpace + 1) == "a")
                registerA *= 3;
            else if (instruc.substr(lastSpace + 1) == "b")
                registerB *= 3;
            else
                throw std::invalid_argument("Given an unexpected register.\n");
        }
        else if (instruction.find("inc") != std::string_view::npos)
        {
            if (instruc.substr(lastSpace + 1) == "a")
                registerA += 1;
            else if (instruc.substr(lastSpace + 1) == "b")
                registerB += 1;
            else
                throw std::invalid_argument("Given an unexpected register.\n");
        }
        else if (instruction.find("jmp") != std::string_view::npos)
        {
            jump(instruc, index, lastSpace);
        }
        else if (instruction.find("jie") != std::string_view::npos)
        {
            std::string reg {findRegister(instruc)};
            if ((reg == "a" && registerA % 2 != 0) || (reg == "b" && registerB % 2 != 0))
                return;
            
            jump(instruc, index, lastSpace);
        }
        else if (instruction.find("jio") != std::string_view::npos)
        {
            std::string reg {findRegister(instruc)};
            if ((reg == "a" && registerA != 1) || (reg == "b" && registerB != 1))
                return;
            
            jump(instruc, index, lastSpace);
        }
        else
            throw std::out_of_range("Given an unexpected instruction.\n");
    }

    template <std::size_t L>
    Register getFinalRegister(const Lines<L>& lines)
    {
        Register registerA {0};
        Register registerB {0};

        Index index {0};
        while (index < L)
        {
            Index prevIndex {index};
            processInstruction(lines.at(index), index, registerA, registerB);
            if (index == prevIndex)
                ++index;
        }

        return registerB;
    }
}

#endif // AOC23A_H
