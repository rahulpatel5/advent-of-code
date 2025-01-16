#ifndef AOC17B_H
#define AOC17B_H

#include <string_view>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
#include <exception>
#include <cassert>

#include <iostream>
#include "../../shared/Timer.h"

/*
re-use part 1 solution
iterate increasing values of register A to generate output
compare generated output to original program
return (i.e. lowest) value for register A where output matches program
consider alternative approach if this takes too long

the program (test and puzzle) halting depend on the loop condition
this depends on the value of register A
looking at the opcodes, register A is only directly affected by 3
if the value of this is <4, there will be a pattern to the output size
we can use this to limit the number of iterations where the output size will be an equivalent size to the program

can add comparison of checks while generating output
i.e. stop iterations early where they already fail

given large number of loops being run, need to do something to reduce time
would there be any pattern in the output?

instead of generating the output, would it make more sense to start with the final output and work backwards from there?
no - int division means specificity is lost
but still have an idea of the order of magnitude e.g. A < 8 for last loop?

instead of going in order (+1), can we dynamically change this?
e.g. multiply by 8 if size too small, or narrowing range if between
will be performance cost from not ending program early

try the previous mentioned reverse approach, i.e. going backwards
even if specificity lost by division, we can generate a range
this should give us a range to use for the starting A register
hopefully this range is smaller than what we're currently looking through

the problem is we can approximately track register A backwards
but we also need to know registers B and C
those are 0 at the start, but we can only work forwards with those
can we work forwards with the program output?

perhaps we can try something akin to a binary search?
the end elements of the output don't change often
so we can get the output at a mid-point and check if it matches
then continue by looking at the next earliest element
*/

namespace aoc17b
{
    template <std::size_t N>
    std::map<char, long long> readRegisters(const std::array<std::string_view, N>& lines)
    {
        std::map<char, long long> registers {};
        for (std::string_view line : lines)
        {
            if (line.find("Register ") == std::string_view::npos)
                continue;
            size_t colon {line.find(':')};
            long long num {std::stoll(std::string(line.substr(colon + 2)))};
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

    long long getComboOperand(int pointer, const std::vector<int>& program, const std::map<char, long long>& registers)
    {
        long long literalOperand {program[pointer + 1]};
        if (literalOperand < 4)
            return literalOperand;
        else if (literalOperand < 7)
            return registers.at('A' + (literalOperand - 4));
        else if (literalOperand == 7)
            throw std::invalid_argument("Invalid program: operand 7 reached.\n");
        else
            throw std::invalid_argument("Received unexpected operand.\n");
    }

    bool isOutputACopyProgram(const std::vector<int>& output, const std::vector<int>& program)
    {
        for (size_t i{0}; i < output.size(); ++i)
        {
            if (output[i] != program[i])
                return false;
        }
        return true;
    }

    std::vector<int> runProgram(std::map<char, long long> registers, const std::vector<int>& program)
    {
        std::vector<int> output {};
        size_t pointer {0};
        // think it makes more sense to keep all this together
        // if this were a longer project, might be neater to send to functions
        // std::cout << "start: " << registers.at('A') << ' ' << registers.at('B') << ' ' << registers.at('C') << '\n';
        while (pointer < program.size() && output.size() <= program.size())
        {
            long long literalOperand {program[pointer + 1]};
            long long comboOperand {getComboOperand(pointer, program, registers)};
            // opcode 0 (adv)
            if (program[pointer] == 0)
            {
                registers['A'] /= std::pow(2, comboOperand);
                // std::cout << "0: " << registers.at('A') << '\n';
            }
            // opcode 1 (bxl)
            else if (program[pointer] == 1)
            {
                registers['B'] ^= literalOperand;
                // std::cout << "1: " << registers.at('B') << '\n';
            }
            // opcode 2 (bst)
            else if (program[pointer] == 2)
            {
                registers['B'] = comboOperand % 8;
                // std::cout << "2: " << registers.at('B') << '\n';
            }
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
            {
                registers['B'] ^= registers['C'];
                // std::cout << "4: " << registers.at('B') << '\n';
            }
            // opcode 5 (out)
            else if (program[pointer] == 5)
            {
                output.push_back(comboOperand % 8);
                // std::cout << "5: " << registers.at('B') % 8 << '\n';
                // early exit if added element doesn't match program
                // if (!isOutputACopyProgram(output, program))
                // if (output[output.size() - 1] != program[output.size() - 1])
                //     return {};
            }
            // opcode 6 (bdv)
            else if (program[pointer] == 6)
            {
                registers['B'] = registers['A'] / std::pow(2, comboOperand);
                // std::cout << "6: " << registers.at('B') << '\n';
            }
            // opcode 7 (cdv)
            else if (program[pointer] == 7)
            {
                registers['C'] = registers['A'] / std::pow(2, comboOperand);
                // std::cout << "7: " << registers.at('C') << '\n';
            }

            // move pointer
            pointer += 2;
        }
        return output;
    }

    std::pair<long long, long long> getCopyProgramLimits(const std::map<char, long long>& registers, const std::vector<int>& program)
    {
        long long divisor {};
        int count {0};
        for (size_t i{0}; i < program.size(); ++i)
        {
            if (program[i] == 0 && i % 2 == 0)
            {
                divisor = program[i + 1];
                ++count;
            }
        }
        // this doesn't work if the divisor is a register (>3)
        // more complex if there's more than one divisor, so skipping that
        // setting default to 10 million loops
        long long defaultLoops {10'000'000};
        if (divisor > 3 || count > 1)
            return {0, defaultLoops};
        

        // adding 0.5 to account for std::pow outputting double
        // based on this post: https://stackoverflow.com/questions/7094108/how-to-raise-an-int-or-long-to-a-power-in-c
        long long lowerBound {(long long)(0.5 + std::pow(2, divisor * (program.size() - 1)))};
        long long upperBound {(long long)(0.5 + std::pow(2, divisor * program.size()))};
        return {lowerBound, upperBound};
    }

    long long findCopyProgram(const std::map<char, long long>& registers, const std::vector<int>& program)
    {
        long long origRegisterAValue {registers.at('A')};
        // find approriate number of iterations, with default 100 million loops
        std::pair<long long, long long> loops {getCopyProgramLimits(registers, program)};
        // std::cout << "from " << loops.first << " to " << loops.second << '\n';
        // loops = {46187030, 46187031};
        long long valueRegisterA {loops.first};
        while (valueRegisterA < loops.second)
        {
            std::map<char, long long> copyRegisters {registers};
            copyRegisters['A'] = valueRegisterA;
            std::vector<int> output {runProgram(copyRegisters, program)};

            // if (valueRegisterA % 10'000 == 0)
            // {
            //     std::cout << valueRegisterA << ' ' << output.size() << '\n';
            //     for (int n : output)
            //         std::cout << n << ',';
            //     std::cout << '\n';
            // }
            
            // adjust if output size is too low or high
            // need to figure out a good way to do this
            if (output.size() < program.size())
                valueRegisterA = (loops.second + valueRegisterA) / 2;
            else if (output.size() > program.size())
                valueRegisterA = (valueRegisterA + loops.first) / 2;

            if (output.size() == program.size() && isOutputACopyProgram(output, program))
                return valueRegisterA;
            ++valueRegisterA;
            if (valueRegisterA == origRegisterAValue)
                ++valueRegisterA;
            // std::cout << "at: " << valueRegisterA << ' ' << output.size() << '\n';
            // if (valueRegisterA % 100'000 == 0)
            // {
            //     std::cout << valueRegisterA << ": ";
            //     for (long long n : output)
            //         std::cout << n << ' ';
            //     std::cout << '\n';
            // }
        }
        // throw std::out_of_range("Failed to reach solution before the set number of loops.\n");
        std::cout << "Failed to reach solution before the set number of loops.\n";
        return 0;
    }

    char getRegisterOutput(const std::vector<int>& program)
    {
        char reg {};
        // we assume there is only one output per loop
        for (size_t i{0}; i < program.size(); i + 2)
        {
            // find the instruction that prints the output
            if (program.at(i) == 5)
            {
                if (program.at(i + 1) == 'A')
                    return 'A';
                else if (program.at(i + 1) == 'B')
                    return 'B';
                else if (program.at(i + 1) == 'C')
                    return 'C';
            }
        }
        throw std::out_of_range("Valid register not found for program output.\n");
    }

    std::pair<long long, long long> getReverseComboOperand(long long literalOperand, const std::pair<long long, long long>& registerA, const std::pair<long long, long long>& registerB, const std::pair<long long, long long>& registerC)
    {
        if (literalOperand < 4)
            return {literalOperand, literalOperand};
        else if (literalOperand == 4)
            return registerA;
        else if (literalOperand == 5)
            return registerB;
        else if (literalOperand == 6)
            return registerC;
        else if (literalOperand == 7)
            throw std::invalid_argument("Invalid program: operand 7 reached.\n");
        else
            throw std::invalid_argument("Received unexpected operand.\n");
    }

    std::pair<long long, long long> getProgramLimits(const std::map<char, long long>& registers, const std::vector<int>& program)
    {
        // get bounds for output
        std::pair<long long, long long> loops {getCopyProgramLimits(registers, program)};

        // to reach end of program, need register A to be 0
        // in last iteration, register A needs to start min 1 to max 7
        std::pair<long long, long long> registerA {1, 7};
        std::pair<long long, long long> registerB {};
        std::pair<long long, long long> registerC {};

        // find which register is the one that outputs
        char registerOutput {getRegisterOutput(program)};
        std::pair<long long, long long>* ptrOutput {};
        if (registerOutput == 'A')
            ptrOutput = &registerA;
        else if (registerOutput == 'B')
            ptrOutput = &registerB;
        else if (registerOutput == 'C')
            ptrOutput = &registerC;
        
        assert(ptrOutput && "Pointer has not been changed correctly and is still a null pointer.\n");

        // need to collect outputs in reverse
        // take program in reverse order
        for (int i{static_cast<int>(program.size()) - 1}; i >= 0; --i)
        {
            std::cout << "outer " << i << '\n';
            // we also need to run the program in reverse
            for (int pointer{static_cast<int>(program.size()) - 1}; pointer > 0; pointer - 2)
            {
                std::cout << "inner " << pointer << '\n';
                int literalOperand {program.at(pointer)};
                std::pair<long long, long long> comboOperand {getReverseComboOperand(literalOperand, registerA, registerB, registerC)};
                // opcode 0
                if (program.at(pointer - 1) == 0)
                {
                    registerA.first *= std::pow(2, comboOperand.first);
                    registerA.second = (registerA.second + 1) * std::pow(2, comboOperand.second) - 1;
                }
                // opcode 1
                else if (program.at(pointer - 1) == 1)
                {
                    // XOR should work in reverse
                    registerB.first ^= literalOperand;
                    registerB.second ^= literalOperand;
                }
                // opcode 2
                else if (program.at(pointer - 1) == 2)
                {
                    // having bounds may fall down e.g. here
                    // do we need to do the inverse of modulo?
                    // is it even possible / feasible to do that?
                    registerB.first = comboOperand.first % 8;
                    registerB.second = comboOperand.second % 8;
                }
                // we've identified register A values giving right loops
                // so we don't care about this instruction
                // // opcode 3 (exit)
                // else if (program.at(pointer - 1) == 3)
                //     {/* intentionally empty */}
                // opcode 4
                else if (program.at(pointer - 1) == 4)
                {
                    registerB.first ^= registerC.first;
                    registerB.second ^= registerC.second;
                }
                // opcode 5
                else if (program.at(pointer - 1) == 5)
                {
                    // ADD HERE
                    // do we need to correct for output being off?
                    // see previous comment about inverse of modulo
                    if (ptrOutput->first % 8 != program.at(i))
                    {
                        ptrOutput->first += (ptrOutput->first % 8 - program.at(i));
                    }
                    if (ptrOutput->second % 8 != program.at(i))
                    {
                        ptrOutput->second += (ptrOutput->second % 8 - program.at(i));
                    }
                }
                // opcode 6
                else if (program.at(pointer - 1) == 6)
                {
                    registerB.first = registerA.first * std::pow(2, comboOperand.first);
                    registerB.second = registerA.second * std::pow(2, comboOperand.second);
                }
                // opcode 7
                else if (program.at(pointer - 1) == 7)
                {
                    registerC.first = registerA.first * std::pow(2, comboOperand.first);
                    registerC.second = registerA.second * std::pow(2, comboOperand.second);
                }
            }
        }
        return registerA;
    }

    template <std::size_t N>
    long long parseAndGetCopyProgram(const std::array<std::string_view, N> lines)
    {
        std::map<char, long long> registers {readRegisters<N>(lines)};
        std::vector<int> program {readProgram<N>(lines)};
        // std::pair<long long, long long> limits {getProgramLimits(registers, program)};
        // std::cout << "limits: " << limits.first << ' ' << limits.second << '\n';
        return findCopyProgram(registers, program);
        // return 0;
    }
}

#endif // AOC17B_H
