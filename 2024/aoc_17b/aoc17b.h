#ifndef AOC17B_H
#define AOC17B_H

#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>

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

found a solution, but it must be larger than the smallest possible one
need to find a way to change the method to get out a smaller value
could just make all references to lower bound increase slower
(and decrease faster?)
could increase the threshold, as assume matches are in a narrowish range

used range to set adjustment
makes the solution a bit more flexible (rather than using magic numbers)
but still not particularly satisfying
would like to figure out a more reliable way of solving this one
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

    Program runProgram(Registers registers, const Program& program)
    {
        Program output {};
        size_t pointer {0};
        // think it makes more sense to keep all this together
        // if this were a longer project, might be neater to send to functions
        while (pointer < program.size() && output.size() <= program.size())
        {
            OperandInt literalOperand {program[pointer + 1]};
            OperandInt comboOperand {getComboOperand(pointer, program, registers)};
            // opcode 0 (adv)
            if (program[pointer] == 0)
            {
                registers['A'] /= std::pow(2, comboOperand);
            }
            // opcode 1 (bxl)
            else if (program[pointer] == 1)
            {
                registers['B'] ^= literalOperand;
            }
            // opcode 2 (bst)
            else if (program[pointer] == 2)
            {
                registers['B'] = comboOperand % 8;
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
            }
            // opcode 5 (out)
            else if (program[pointer] == 5)
            {
                output.push_back(comboOperand % 8);
            }
            // opcode 6 (bdv)
            else if (program[pointer] == 6)
            {
                registers['B'] = registers['A'] / std::pow(2, comboOperand);
            }
            // opcode 7 (cdv)
            else if (program[pointer] == 7)
            {
                registers['C'] = registers['A'] / std::pow(2, comboOperand);
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

    int countProgramMatches(const Program& output, const Program& program)
    {
        int count {0};
        for (int i{static_cast<int>(program.size() - 1)}; i >= 0; --i)
        {
            if (output.at(i) == program.at(i))
                ++count;
            else
                break;
        }
        return count;
    }

    RegisterInt searchForSolution(const Registers& registers, const Program& program, const Limits& absoluteLimits, double adjust, RegisterInt threshold)
    {
        Program output {};
        // set the initial bounds we'll be using
        Limit lowerBound {absoluteLimits.first};
        Limit upperBound {absoluteLimits.second};
        RegisterInt midpoint {};
        while (output != program)
        {
            // if we're under the threshold, run the rest
            if (upperBound - lowerBound <= threshold)
            {
                for (RegisterInt n{lowerBound}; n <= upperBound; ++n)
                {
                    Registers finalRegisters {registers};
                    finalRegisters['A'] = n;
                    Program finalOutput {runProgram(finalRegisters, program)};
                    if (finalOutput == program)
                        return n;
                }
                // return sentinel value if no solution found
                return 0;
                // throw std::out_of_range("Solution was not found (threshold stage).\n");
            }

            Limit tempLowerBound {lowerBound};
            Limit tempUpperBound {upperBound};

            // this is inefficent
            // would be faster to run only if lower/upper point change
            Registers copyRegisters {registers};
            copyRegisters['A'] = tempLowerBound;
            Program lowerOutput {runProgram(copyRegisters, program)};
            copyRegisters = registers;
            copyRegisters['A'] = tempUpperBound;
            Program upperOutput {runProgram(copyRegisters, program)};
            
            Limit midPoint {(tempUpperBound + tempLowerBound) / 2};
            copyRegisters = registers;
            copyRegisters['A'] = midPoint;
            Program midOutput {runProgram(copyRegisters, program)};

            // first need to guard against wrong output size
            if (lowerOutput.size() != program.size() || upperOutput.size() != program.size())
            {
                // assumes lower has smaller or upper has larger size
                if (lowerOutput.size() < program.size())
                    lowerBound += (midPoint - tempLowerBound) * adjust * adjust;
                if (upperOutput.size() > program.size())
                    upperBound -= (tempUpperBound - midPoint) * adjust;
                continue;
            }

            Limit lowerQuarter {static_cast<Limit>(tempLowerBound + (midPoint - tempLowerBound) / 2)};
            Limit upperQuarter {static_cast<Limit>(tempUpperBound - (tempUpperBound - midPoint) / 2)};

            copyRegisters = registers;
            copyRegisters['A'] = lowerQuarter;
            Program lowerQuarterOutput {runProgram(copyRegisters, program)};
            copyRegisters = registers;
            copyRegisters['A'] = upperQuarter;
            Program upperQuarterOutput {runProgram(copyRegisters, program)};

            // see how well each iteration matches program
            int matchesLowerBound {countProgramMatches(lowerOutput, program)};
            int matchesUpperBound {countProgramMatches(upperOutput, program)};
            int matchesMidpoint {countProgramMatches(midOutput, program)};
            int matchesLowerQuarter {countProgramMatches(lowerQuarterOutput, program)};
            int matchesUpperQuarter {countProgramMatches(upperQuarterOutput, program)};

            // might as well check in case we lucked out on any of these
            if (matchesLowerBound == program.size())
                return lowerBound;
            else if (matchesUpperBound == program.size())
                return upperBound;
            else if (matchesMidpoint == program.size())
                return midPoint;
            else if (matchesLowerQuarter == program.size())
                return lowerQuarter;
            else if (matchesUpperQuarter == program.size())
                return upperQuarter;
            
            // now we'll try to narrow the bounds
            if (matchesLowerQuarter > matchesUpperQuarter)
            {
                upperBound = upperQuarter;
            }
            else if (matchesLowerQuarter < matchesUpperQuarter)
            {
                lowerBound = lowerQuarter;
            }
            else if (matchesLowerBound > matchesUpperBound)
            {
                upperBound -= (upperBound - upperQuarter) * adjust;
            }
            else if (matchesLowerBound < matchesUpperBound)
            {
                lowerBound += (lowerQuarter - lowerBound) * adjust * adjust;
            }
            else
            {
                upperBound -= (upperBound - upperQuarter) * adjust;
                // change lower bound slower, as we want smallest solution
                lowerBound += (lowerQuarter - lowerBound) * adjust * adjust;
            }
        }
        throw std::out_of_range("Did not find a solution.\n");
    }

    template <std::size_t N>
    RegisterInt parseAndGetCopyProgram(const std::array<std::string_view, N> lines)
    {
        Registers registers {readRegisters<N>(lines)};
        Program program {readProgram<N>(lines)};

        // find absolute limits for loops
        Limits absoluteLimits {getCopyProgramLimits(registers, program)};

        RegisterInt smallest {};
        // threshold below which we run all remaining values
        RegisterInt threshold {50'000};
        // set how quickly we change the limits, between 0 and 1
        // closer to 0 takes longer to change, and may get stuck in valleys
        // closer to 1 takes less time to change, but may miss values
        for (double adjust{0.10}; adjust < 0.40; adjust += 0.01)
        {
            RegisterInt solution {searchForSolution(registers, program, absoluteLimits, adjust, threshold)};
            if (solution > 0 && (smallest == 0 || solution < smallest))
                smallest = solution;
        }
        
        return smallest;
    }
}

#endif // AOC17B_H
