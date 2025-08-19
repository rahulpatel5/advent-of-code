#ifndef AOC17B_H
#define AOC17B_H

#include <string_view>
#include <array>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <tuple>
// #include <algorithm>

#include <iostream>

/*
work backwards from the end state

FIND usual order of operation
START with known state: A must be a certain value to end program

RUN program in reverse
FOR each iteration, starting with end loop
    TRACK the value, or range of values, that each register may be
    USE output (in reverse) to get info on register value(s)
    USE (reverse) modulo and division operations to get more info
    ASSUME lowest value possible, initially

RETURN lowest starting value for A

working directly backwards doesn't give the right solution
need to correct for remainder calculations when we get more info

collect values in current loop and roll back if reach a point where
a register doesn't have a value it should
this will require storing functions to re-run
*/

namespace aoc17b
{
    using RegisterInt = long long;
    using Register = char;
    using Registers = std::map<Register, RegisterInt>;
    using Program = std::vector<int>;
    using OperandInt = long long;

    using Instruction = int;
    // index 0-2: register A-C, index 3-4: literal/combo operand,
    // index 5: instruction
    using Values = std::tuple<RegisterInt, RegisterInt, RegisterInt, OperandInt, OperandInt, Instruction>;
    using ValueCollection = std::vector<Values>;

    using Index = size_t;

    // using RegisterRange = std::pair<OperandInt, OperandInt>;
    // using RegisterRanges = std::map<Register, RegisterRange>;
    // // value assigned when we don't know what register value to use
    // constexpr OperandInt UNKNOWN_REGISTER_VALUE {-1};

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
            Register reg {line.data()[firstSpace + 1]};
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
            comma = line.find(',', ++comma);
        }
        return program;
    }

    // checks some assumptions we make to simplify things
    void checkProgram(const Program& program)
    {
        assert(program.size() % 2 == 0 && "The program did not have an even number of elements.\n");

        bool is_adv {false};
        bool is_out {false};
        for (size_t i{0}; i < program.size(); i += 2)
        {
            // we'll assume that the last operation is 3
            // processing things is more complex if it isn't
            if (program.at(i) == 3 && i != program.size() - 2)
            {
                throw std::out_of_range("Expected the jnz operation to only occur last, but it occurred before.\n");
            }

            // there must be an adv operation or things don't work
            if (program.at(i) == 0)
                is_adv = true;
            // there must be an out operation or things don't work
            if (program.at(i) == 5)
                is_out = true;
        }

        // we'll assume that the last operand is 0
        // it gets more complex if it isn't
        assert(program.back() == 0 && "The last element isn't 0.\n");
    }

    // opcode 0
    void adv(Registers& registers, OperandInt comboOperand)
    {
        registers['A'] = ((registers['A'] == 0) ? 1 : registers['A'] * std::pow(2, comboOperand));
    }
    
    // opcode 1
    void bxl(Registers& registers, OperandInt literalOperand)
    {
        registers['B'] ^= literalOperand;
    }
    
    // opcode 2
    void bst(Registers& registers, OperandInt comboOperand)
    {
        // add handling to roll back where register B != combo % 8
        
        // assert(registers['B'] == comboOperand % 8);
        
        // if (registers['B'] != comboOperand % 8)
        // {
        //     updateRegister(registers, literalOperand, comboOperand, registers['B'], 8);
        // }
    }
    
    // opcode 3
    void jnz()
    {
        /* deliberately empty */
    }
    
    // opcode 4
    void bxc(Registers& registers)
    {
        registers['B'] ^= registers['C'];
    }
    
    // opcode 5
    void out()
    {
        /* deliberately empty */
    }
    
    // opcode 6
    void bdv(Registers& registers, OperandInt comboOperand)
    {
        // DIVISION OR MULTIPLICATION?
        registers['B'] = registers['A'] / std::pow(2, comboOperand);
    }
    
    // opcode 7
    void cdv(Registers& registers, OperandInt comboOperand)
    {
        // DIVISION OR MULTIPLICATION?
        registers['C'] = registers['A'] / std::pow(2, comboOperand);
    }

    void doInstruction(Instruction instruction, Registers& registers, OperandInt literalOperand, OperandInt comboOperand)
    {
        switch (instruction)
        {
        case 0:  return adv(registers, comboOperand);
        case 1:  return bxl(registers, literalOperand);
        case 2:  return bst(registers, comboOperand);
        case 3:  return jnz();
        case 4:  return bxc(registers);
        case 5:  return out();
        case 6:  return bdv(registers, comboOperand);
        case 7:  return cdv(registers, comboOperand);
        default: throw std::out_of_range("Given non-valid instruction.\n");
        }
    }

    OperandInt getComboOperand(int pointer, const Program& reverseProgram, const Registers& registers)
    {
        OperandInt literalOperand {reverseProgram[pointer - 1]};
        if (literalOperand < 4)
            return literalOperand;
        else if (literalOperand < 7)
            return registers.at('A' + (literalOperand - 4));
        else if (literalOperand == 7)
            throw std::invalid_argument("Invalid program: operand 7 reached.\n");
        else
            throw std::invalid_argument("Received unexpected operand.\n");
    }

    Register getComboOperandRegister(OperandInt literalOperand)
    {
        if (literalOperand >= 4 && literalOperand <= 6)
        {
            return 'A' + (literalOperand - 4);
        }
        else
            throw std::invalid_argument("Expected to get a register.\n");
    }

    // void updateRegister(Registers& registers, OperandInt literal, OperandInt compound, int remainder, int modulo)
    // {
    //     assert((literal > 3 && literal < 7) && "Register doesn't have an expected letter.\n");
    //     Register reg {static_cast<Register>('A' + (literal - 4))};
    //     assert(registers[reg] == compound && "Register value did not match value for compound literal.\n");
    //     registers[reg] += (remainder - (compound % modulo));
    // }

    RegisterInt getRegisterValue(Register reg, const Values& values)
    {
        if (reg == 'A')
            return std::get<0>(values);
        else if (reg == 'B')
            return std::get<1>(values);
        else if (reg == 'C')
            return std::get<2>(values);
        else
            throw std::invalid_argument("Given unexpected register.\n");
    }

    Index findLastChange(const ValueCollection& memory, Register wrongRegister)
    {
        Index index {memory.size() - 1};
        RegisterInt finalValue {getRegisterValue(wrongRegister, memory.back())};
        std::cout << "\t\t\twrong register: " << wrongRegister << '\n';
        for (auto revIt{memory.rbegin()}; revIt != memory.rend(); ++revIt)
        {
            std::cout << "\t\t\t register A: " << std::get<0>(*revIt) << '\n';
            std::cout << "\t\t\t register B: " << std::get<1>(*revIt) << '\n';
            std::cout << "\t\t\t register C: " << std::get<2>(*revIt) << '\n';
            RegisterInt newValue {getRegisterValue(wrongRegister, *revIt)};
            if (newValue != finalValue)
                // want instruction after change
                return index + 1;
            assert(index > 0 && "Expected index to be >0 before reducing.\n");
            --index;
        }
        throw std::out_of_range("Did not find where the register's value last changed.\n");
    }

    void rerunRemainingInstructions(Registers& registers, const ValueCollection& memory, Index previousIndex)
    {
        for (size_t i{previousIndex}; i < memory.size(); ++i)
        {
            doInstruction(std::get<5>(memory[i]), registers, std::get<3>(memory[i]), std::get<4>(memory[i]));

            std::cout << "\t\tA: " << registers['A'] << '\n';
            std::cout << "\t\tB: " << registers['B'] << '\n';
            std::cout << "\t\tC: " << registers['C'] << '\n';
        }
    }

    // assumes only the most recent change was wrong
    // and that only one register needs to be corrected
    void correctAndRerunInstructions(Registers& registers, const ValueCollection& memory, Register wrongRegister, RegisterInt correctedValue)
    {
        std::cout << "\tCORRECTING\n";

        std::cout << "\t\tinitial A: " << registers['A'] << '\n';
        std::cout << "\t\tinitial B: " << registers['B'] << '\n';
        std::cout << "\t\tinitial C: " << registers['C'] << '\n';

        Index previousIndex {findLastChange(memory, wrongRegister)};
        // revert register values
        registers['A'] = std::get<0>(memory[previousIndex]);
        registers['B'] = std::get<1>(memory[previousIndex]);
        registers['C'] = std::get<2>(memory[previousIndex]);

        std::cout << "\t\trevert A: " << registers['A'] << '\n';
        std::cout << "\t\trevert B: " << registers['B'] << '\n';
        std::cout << "\t\trevert C: " << registers['C'] << '\n';

        // correct value
        registers[wrongRegister] = correctedValue;
        std::cout << "\t\tfix " << wrongRegister << ": " << registers[wrongRegister] << '\n';
        // now re-run the rest of the instructions
        rerunRemainingInstructions(registers, memory, previousIndex);
    }

    RegisterInt runReverseProgram(Registers origRegisters, const Program& reverseProgram)
    {
        Registers registers {origRegisters};

        for (int i{0}; i < reverseProgram.size(); ++i)
        {
            ValueCollection memory {};

            std::cout << '\t' << i << ' ' << reverseProgram.at(i) << '\n';
            for (int j{1}; j < reverseProgram.size(); j += 2)
            {
                OperandInt literalOperand {reverseProgram[j - 1]};
                OperandInt comboOperand {getComboOperand(j, reverseProgram, registers)};

                memory.push_back(std::make_tuple(registers['A'], registers['B'], registers['C'], literalOperand, comboOperand, reverseProgram[j]));

                // opcode 0 (adv)
                if (reverseProgram[j] == 0)
                {
                    adv(registers, comboOperand);

                    std::cout << "0: " << 'A' << registers['A'] << ' ' << std::pow(2, comboOperand) << '\n';
                    assert(registers['A'] >= 0 && "reg B0 wrong");
                }
                // opcode 1 (bxl)
                else if (reverseProgram[j] == 1)
                {
                    bxl(registers,literalOperand);

                    std::cout << "1: " << 'B' << registers['B'] << ' ' << literalOperand << '\n';
                    assert(registers['B'] >= 0 && "reg A0 wrong");
                }
                // opcode 2 (bst)
                else if (reverseProgram[j] == 2)
                {
                    if (registers['B'] != comboOperand % 8)
                    {
                        Register wrongRegister {'B'};
                        // RegisterInt correctedValue {(comboOperand % 8) - registers['B']};
                        RegisterInt correctedValue {(comboOperand % 8)};
                        correctAndRerunInstructions(registers, memory, wrongRegister, correctedValue);

                        // updateRegister(registers, literalOperand, comboOperand, registers['B'], 8);
                    }

                    bst(registers, comboOperand);

                    std::cout << "2: " << 'B' << registers['B'] << ' ' << comboOperand << '\n';
                    assert(registers['B'] >= 0 && "reg B1 wrong");
                }
                // opcode 3 (jnz)
                else if (reverseProgram[j] == 3)
                {
                    // we assume this operation is never relevant
                    // since we're working in reverse
                    jnz();

                    assert((i == 0 || registers['A'] > 0) && "Register A can't be 0 before last turn.\n");
                    std::cout << "3: " << 'A' << registers['A'] << '\n';
                }
                // opcode 4 (bxc)
                else if (reverseProgram[j] == 4)
                {
                    bxc(registers);

                    std::cout << "4: " << 'B' << registers['B'] << " C" << registers['C'] << '\n';
                    assert(registers['B'] >= 0 && "reg B2 wrong");
                }
                // opcode 5 (out)
                else if (reverseProgram[j] == 5)
                {
                    out();

                    if (comboOperand % 8 != reverseProgram.at(i))
                    {
                        Register wrongRegister {getComboOperandRegister(literalOperand)};
                        // RegisterInt correctedValue {(comboOperand % 8) - reverseProgram[i]};
                        RegisterInt correctedValue {(comboOperand % 8)};
                        correctAndRerunInstructions(registers, memory, wrongRegister, correctedValue);
                    }
                    std::cout << "5: " << 'B' << registers['B'] << ' ' << reverseProgram.at(j) << '\n';
                }
                // opcode 6 (bdv)
                else if (reverseProgram[j] == 6)
                {
                    // DIVISION OR MULTIPLICATION?
                    bdv(registers, comboOperand);

                    std::cout << "6: " << 'B' << registers['B'] << " A" << registers['A'] << ' ' << std::pow(2, comboOperand) << '\n';
                    assert(registers['B'] >= 0 && "reg B3 wrong");
                }
                // opcode 7 (cdv)
                else if (reverseProgram[j] == 7)
                {
                    // DIVISION OR MULTIPLICATION?
                    cdv(registers, comboOperand);

                    std::cout << "7: " << 'C' << registers['C'] << " A" << registers['A'] << ' ' << std::pow(2, comboOperand) << '\n';
                    assert(registers['C'] >= 0 && "reg C0 wrong");
                }
            }
        }

        return registers['A'];
    }

    template <std::size_t N>
    RegisterInt parseAndGetCopyProgram(const std::array<std::string_view, N> lines)
    {
        Registers registers {readRegisters<N>(lines)};
        Program program {readProgram<N>(lines)};

        checkProgram(program);

        Program reverseProgram (program.rbegin(), program.rend());

        // we're starting in the end state and working back
        // we know (from the logic) that A must be 0 in the end
        registers['A'] = 0;

        return runReverseProgram(registers, reverseProgram);
    }
}

#endif // AOC17B_H
