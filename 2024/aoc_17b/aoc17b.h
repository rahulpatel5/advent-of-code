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
need to be careful - output doesn't change predictably
can't expect sequential changes e.g. want 3 so too high if 4
we just want to reduce the number of loops that we have to go through

algorithm for getting limit
1. start with last element of output
2. get output for lower and upper bounds
3. if bounds just outside where equal to last element, proceed to 5
4. otherwise adjust (usually reduce) the bounds and go back to 2
5. move to previous element of output and repeat 2-4
6. when have iterated over half of elements, or limit is <threshold, stop

using bounds doesn't seem to work like this
it may be that matching elements changes too frequently
need to be more precise with how algorithm settings are set
or try another approach e.g. directly iterating until reaching the target
*/

namespace aoc17b
{
    using RegisterInt = long long;
    using Registers = std::map<char, RegisterInt>;
    using Program = std::vector<int>;
    using Limit = long long;
    using Limits = std::pair<Limit, Limit>;
    using OperandInt = long long;

    template <std::size_t N>
    Registers readRegisters(const std::array<std::string_view, N>& lines)
    {
        Registers registers {};
        for (std::string_view line : lines)
        {
            if (line.find("Register ") == std::string_view::npos)
                continue;
            size_t colon {line.find(':')};
            RegisterInt num {std::stoll(std::string(line.substr(colon + 2)))};
            size_t firstSpace {line.find(' ')};
            char reg {line.data()[firstSpace + 1]};
            registers[reg] = num;
        }
        return registers;
    }

    template <std::size_t N>
    Program readProgram(const std::array<std::string_view, N>& lines)
    {
        Program program {};

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

    OperandInt getComboOperand(int pointer, const Program& program, const Registers& registers)
    {
        OperandInt literalOperand {program[pointer + 1]};
        if (literalOperand < 4)
            return literalOperand;
        else if (literalOperand < 7)
            return registers.at('A' + (literalOperand - 4));
        else if (literalOperand == 7)
            throw std::invalid_argument("Invalid program: operand 7 reached.\n");
        else
            throw std::invalid_argument("Received unexpected operand.\n");
    }

    bool isOutputACopyProgram(const Program& output, const Program& program)
    {
        for (size_t i{0}; i < output.size(); ++i)
        {
            if (output[i] != program[i])
                return false;
        }
        return true;
    }

    Program runProgram(Registers registers, const Program& program)
    {
        Program output {};
        size_t pointer {0};
        // think it makes more sense to keep all this together
        // if this were a longer project, might be neater to send to functions
        // std::cout << "start: " << registers.at('A') << ' ' << registers.at('B') << ' ' << registers.at('C') << '\n';
        while (pointer < program.size() && output.size() <= program.size())
        {
            OperandInt literalOperand {program[pointer + 1]};
            OperandInt comboOperand {getComboOperand(pointer, program, registers)};
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

    Limits getCopyProgramLimits(const Registers& registers, const Program& program)
    {
        Limit divisor {};
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
        Limit defaultLoops {10'000'000};
        if (divisor > 3 || count > 1)
            return {0, defaultLoops};
        

        // adding 0.5 to account for std::pow outputting double
        // based on this post: https://stackoverflow.com/questions/7094108/how-to-raise-an-int-or-long-to-a-power-in-c
        Limit lowerBound {(Limit)(0.5 + std::pow(2, divisor * (program.size() - 1)))};
        Limit upperBound {(Limit)(0.5 + std::pow(2, divisor * program.size()))};
        return {lowerBound, upperBound};
    }

    Limits getRefinedLimits(const Registers& registers, const Program& program, const Limits& absoluteLimits)
    {
        // start with last element
        size_t element {program.size() - 1};
        // we'll find limits based on half the size of the program
        // add one more element as currently not limiting range enough
        size_t elementIter {program.size() / 2 - 1};
        // set the bounds we'll be using
        Limit lowerBound {absoluteLimits.first};
        Limit upperBound {absoluteLimits.second};
        // we'll set a threshold where we're satisfied with the bounds
        Limit threshold {1'000'000};
        if (upperBound - lowerBound <= threshold)
            return {lowerBound, upperBound};
        // set how quickly we change the limits
        // between 0 and 1. Closer to 0 takes longer to change
        double adjust {0.2};
        // std::cout << "to: " << elementIter << '\n';
        while (element >= elementIter)
        {
            // std::cout << element << ' ' << program.at(element) << '\n';
            Limit innerLowerBound {lowerBound};
            Limit innerUpperBound {upperBound};
            bool isComplete {false};
            while (!isComplete)
            {
                // this is inefficent
                // would be faster to run only if lower/upper point change
                Registers copyRegisters {registers};
                copyRegisters['A'] = innerLowerBound;
                Program lowerOutput {runProgram(copyRegisters, program)};
                copyRegisters = registers;
                copyRegisters['A'] = innerUpperBound;
                Program upperOutput {runProgram(copyRegisters, program)};
                
                Limit midPoint {(innerUpperBound + innerLowerBound) / 2};
                copyRegisters = registers;
                copyRegisters['A'] = midPoint;
                Program midOutput {runProgram(copyRegisters, program)};

                // std::cout << innerLowerBound << ' ' << midPoint << ' ' << innerUpperBound << '\n';
                // std::cout << '\t' << lowerOutput.at(element) << ' ' << midOutput.at(element) << ' ' << upperOutput.at(element) << ' ' << program.at(element) << '\n';
                // std::cout << '\t';
                // for (int n : midOutput)
                //     std::cout << n << ' ';
                // std::cout << '\n';

                // first need to guard against wrong output size
                if (lowerOutput.size() != program.size() || upperOutput.size() != program.size())
                {
                    // not handling situation where lower output is larger
                    // or where upper output is smaller
                    if (lowerOutput.size() < program.size())
                        innerLowerBound = (midPoint + innerLowerBound) / 2;
                    if (upperOutput.size() > program.size())
                        innerUpperBound = (innerUpperBound + midPoint) / 2;
                    continue;
                }

                Limit tempLowerBound {static_cast<Limit>(innerLowerBound + (midPoint - innerLowerBound) * adjust)};
                Limit tempUpperBound {static_cast<Limit>(innerUpperBound - (innerUpperBound - midPoint) * adjust)};
                copyRegisters = registers;
                copyRegisters['A'] = tempLowerBound;
                Program tempLowerOutput {runProgram(copyRegisters, program)};
                copyRegisters = registers;
                copyRegisters['A'] = tempUpperBound;
                Program tempUpperOutput {runProgram(copyRegisters, program)};

                // std::cout << "\t\t" << tempLowerOutput.at(element) << ' ' << tempUpperOutput.at(element) << '\n';

                if (lowerOutput.at(element) != program.at(element) && tempLowerOutput.at(element) == program.at(element) && upperOutput.at(element) != program.at(element) && tempUpperOutput.at(element) == program.at(element))
                {
                    isComplete = true;
                    if (innerUpperBound - innerLowerBound <= threshold)
                        break;
                    continue;
                }

                // how we change bounds can be guided by midpoint
                if (midOutput.at(element) == program.at(element))
                {
                    if (lowerOutput.at(element) != program.at(element))
                    {
                        innerLowerBound += (midPoint - innerLowerBound) * adjust;
                    }
                    else
                    {
                        if (innerLowerBound == lowerBound)
                        {
                            if (upperOutput.at(element) != program.at(element) && tempUpperOutput.at(element) == program.at(element))
                            {
                                isComplete = true;
                                if (innerUpperBound - innerLowerBound <= threshold)
                                    break;
                                continue;
                            }
                        }
                        else
                        {
                            innerLowerBound -= (innerLowerBound - lowerBound) * adjust * adjust;
                        }
                    }
                    
                    if (upperOutput.at(element) != program.at(element))
                    {
                        innerUpperBound -= (innerUpperBound - midPoint) * adjust;
                    }
                    else
                    {
                        if (innerUpperBound == upperBound)
                        {
                            if (lowerOutput.at(element) != program.at(element) && tempLowerOutput.at(element) == program.at(element))
                            {
                                isComplete = true;
                                if (innerUpperBound - innerLowerBound <= threshold)
                                    break;
                                continue;
                            }
                        }
                        else
                        {
                            innerUpperBound += (upperBound - innerUpperBound) * adjust * adjust;
                        }
                    }
                }
                else
                {
                    // std::cout << "-1\n";
                    // if midpoint doesn't match program
                    // that means we shouldn't change bounds symmetrically
                    if (lowerOutput.at(element) != program.at(element) && upperOutput.at(element) != program.at(element))
                    {
                        // std::cout << "0\n";
                        innerLowerBound += (midPoint - innerLowerBound) * adjust;
                        // favour changing upper bound by less
                        innerUpperBound -= (innerUpperBound - midPoint) * adjust * adjust;
                    }
                    else if (lowerOutput.at(element) == program.at(element) && upperOutput.at(element) == program.at(element))
                    {
                        innerLowerBound -= (innerLowerBound - lowerBound) * adjust;
                        // favour changing upper bound by less
                        innerUpperBound += (upperBound - innerUpperBound) * adjust * adjust;
                    }
                    else
                    {
                        // std::cout << "1\n";
                        // if upper bound matches, shift upwards
                        if (lowerOutput.at(element) != program.at(element) && upperOutput.at(element) == program.at(element))
                        {
                            // std::cout << "2\n";
                            innerLowerBound += (midPoint - innerLowerBound) * adjust;
                            innerUpperBound += (upperBound - innerUpperBound) * adjust;
                        }
                        // if lower bound matches, shift downwards
                        else if (lowerOutput.at(element) == program.at(element) && upperOutput.at(element) != program.at(element))
                        {
                            // std::cout << "3\n";
                            innerLowerBound -= (innerLowerBound - lowerBound) * adjust;
                            innerUpperBound += (innerUpperBound - midPoint) * adjust;
                        }
                    }
                }
            }
            
            lowerBound = innerLowerBound;
            upperBound = innerUpperBound;
            --element;
        }
        Registers tempRegisters {registers};
        tempRegisters['A'] = lowerBound;
        Program tempProgramDown {runProgram(tempRegisters, program)};
        tempRegisters = registers;
        tempRegisters['A'] = upperBound;
        Program tempProgramUp {runProgram(tempRegisters, program)};
        // std::cout << "lower: ";
        // for (int n : tempProgramDown)
        //     std::cout << n << ' ';
        // std::cout << '\n';
        // std::cout << "upper: ";
        // for (int n : tempProgramUp)
        //     std::cout << n << ' ';
        // std::cout << '\n';
        return {lowerBound, upperBound};
    }

    RegisterInt findCopyProgram(const Registers& registers, const Program& program, const Limits& limits)
    {
        RegisterInt origRegisterAValue {registers.at('A')};
        RegisterInt valueRegisterA {limits.first};
        while (valueRegisterA < limits.second)
        {
            Registers copyRegisters {registers};
            copyRegisters['A'] = valueRegisterA;
            Program output {runProgram(copyRegisters, program)};

            // if (valueRegisterA % 1'000'000 == 0)
            //     std::cout << valueRegisterA << '\n';
            // {
                // std::cout << valueRegisterA << ": ";
                // for (int n : output)
                //     std::cout << n << ',';
                // std::cout << '\n';
            // }
            
            // adjust if output size is too low or high
            // need to figure out a good way to do this
            // if (output.size() < program.size())
            //     valueRegisterA = (limits.second + valueRegisterA) / 2;
            // else if (output.size() > program.size())
            //     valueRegisterA = (valueRegisterA + limits.first) / 2;

            if (output.size() == program.size() && isOutputACopyProgram(output, program))
                return valueRegisterA;
            ++valueRegisterA;
            if (valueRegisterA == origRegisterAValue)
                ++valueRegisterA;
            // std::cout << "at: " << valueRegisterA << ' ' << output.size() << '\n';
            // if (valueRegisterA % 100'000 == 0)
            // {
            // std::cout << valueRegisterA << ": ";
            // for (long long n : output)
            //     std::cout << n << ' ';
            // std::cout << '\n';
            // }
        }
        // throw std::out_of_range("Failed to reach solution before the set number of loops.\n");
        std::cout << "Failed to reach solution before the set number of loops.\n";
        return 0;
    }

    template <std::size_t N>
    RegisterInt parseAndGetCopyProgram(const std::array<std::string_view, N> lines)
    {
        Registers registers {readRegisters<N>(lines)};
        Program program {readProgram<N>(lines)};

        // find absolute limits for loops
        Limits absoluteLimits {getCopyProgramLimits(registers, program)};
        std::cout << "abs: " << absoluteLimits.first << ' ' << absoluteLimits.second << '\n';
        // refine this further using how the output changes
        Limits refinedLimits {getRefinedLimits(registers, program, absoluteLimits)};
        std::cout << "ref: " << refinedLimits.first << ' ' << refinedLimits.second << '\n';

        // return findCopyProgram(registers, program, refinedLimits);
        return findCopyProgram(registers, program, refinedLimits);
    }
}

#endif // AOC17B_H
