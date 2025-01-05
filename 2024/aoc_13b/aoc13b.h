#ifndef AOC13B_H
#define AOC13B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>
#include <numeric>
#include <limits>
#include <stdexcept>

/*
re-use parts of part 1 solution
logic may be to identify where a set of numbers add up to e.g. 100,000
or the power of 10 above the target x or y value
then multiply that to get to the 10'000'000'000'000?
but this doesn't make sense as test machines 1 and 3 were winnable in part 1
but now test machines 2 and 4 are winnable
this may mean that a higher power of 10 is a multiple of the solution

what happens if I just do standard division, like the part 1 solution?
takes too long

we can speed up by using division instead of adding in getPresses()
doesn't really make a difference
making sure that the final answer is evenly divisible before does work

some speed up from removing presses that are evenly divisible
e.g. no point doing A x2 and B x4 if we've already done A x1 and B x2
could extend that to other numbers, maybe just do 3 and 5

not getting an answer for a number of loops that takes a long time to run
need to think of another approach
can this be turned into a function and then try to find the solution?
we have two equations:
    Abutton.x * Apresses + Bbutton.x * Bpresses == prize.x
    Abutton.y * Apresses + Bbutton.y * Bpresses == prize.y
if we manipulate one equation such that e.g. Bbutton is same as other
then we can subtract the two and end up with a simplified solution
    e.g. n * Abutton.x * Apresses == n * prize.x
we can then solve for Apresses
then we can plug back into an earlier equation to get Bpresses
*/

namespace constants
{
    constexpr long long ERROR {10'000'000'000'000};
}

namespace aoc13b
{
    using TokenInt = long long;
    using PairInt = std::pair<TokenInt, TokenInt>;
    using Machine = std::vector<PairInt>;
    using Machines = std::vector<Machine>;

    template <std::size_t N>
    Machines getMachineConfigurations(const std::array<std::string_view, N>& lines)
    {
        Machine Abuttons {};
        Machine Bbuttons {};
        Machine prizeLocations {};

        std::string_view button {"Button "};
        std::string_view buttonA {"Button A:"};
        std::string_view prizes {"Prize:"};
        for (std::string_view line : lines)
        {
            if (line.find(button) != std::string_view::npos)
            {
                size_t firstPlus {line.find('+')};
                size_t comma {line.find(',')};
                TokenInt x {std::stoi(std::string(line.substr(firstPlus + 1, comma - (firstPlus + 1))))};

                size_t secondPlus {line.find('+', firstPlus + 1)};
                TokenInt y {std::stoi(std::string(line.substr(secondPlus + 1)))};

                if (line.find(buttonA) != std::string_view::npos)
                    Abuttons.push_back({x, y});
                else
                    Bbuttons.push_back({x, y});
            }
            else if (line.find(prizes) != std::string_view::npos)
            {
                size_t firstEqual {line.find('=')};
                size_t comma {line.find(',')};
                TokenInt x {std::stoi(std::string(line.substr(firstEqual + 1, comma - (firstEqual + 1))))};

                size_t secondEqual {line.find('=', firstEqual + 1)};
                TokenInt y {std::stoi(std::string(line.substr(secondEqual + 1)))};

                // add extra for part 2 puzzle
                prizeLocations.push_back({x + constants::ERROR, y + constants::ERROR});
                // prizeLocations.push_back({x, y});
            }
        }
        assert((Abuttons.size() == Bbuttons.size() && Bbuttons.size() == prizeLocations.size()) && "The number of A buttons, B buttons, and prizes are not the same.");

        Machines machines {};
        for (size_t i{0}; i < Abuttons.size(); ++i)
            machines.push_back({Abuttons[i], Bbuttons[i], prizeLocations[i]});
        return machines;
    }

    // based on: https://stackoverflow.com/questions/199333/how-do-i-detect-unsigned-integer-overflow
    void isIntegerOverflow(TokenInt num, TokenInt multiplier)
    {
        if (num == 0 || multiplier == 0)
            throw std::invalid_argument("Division by zero error.\n");
        if (num > LLONG_MAX / multiplier)
            throw std::out_of_range("Calculation would cause an integer overflow.\n");
    }

    void confirmCalculation(TokenInt Apresses, TokenInt Bpresses, const PairInt& Abutton, const PairInt& Bbutton, const PairInt& prize)
    {
        if ((Apresses * Abutton.first + Bpresses * Bbutton.first != prize.first) || (Apresses * Abutton.second + Bpresses * Bbutton.second != prize.second))
            throw std::invalid_argument("Calculation was calculated wrong.\n");
    }

    template <TokenInt A, TokenInt B>
    TokenInt solveForButtonCombinations(const Machine& machine)
    {
        PairInt Abutton {machine[0]};
        PairInt Bbutton {machine[1]};
        PairInt prize {machine[2]};

        // we have two equations:
        // Abutton.x * a + Bbutton.x * b == prize.x
        // Abutton.y * a + Bbutton.y * b == prize.y
        // we have values for Abutton and prize
        // so a and b are the unknowns
        // but we have two equations so we can take the difference
        // we'll choose to make Bbutton the same

        // get the lowest common denominator for the Bbutton values
        TokenInt lcm {std::lcm(Bbutton.first, Bbutton.second)};

        // now we get the adjusted Abutton values
        // but first check if the equations might not have a solution
        if (((Abutton.first * lcm) % Bbutton.first != 0) || ((Abutton.second * lcm) % Bbutton.second != 0))
            return 0;

        // think we're with long long, but let's check for overflow
        isIntegerOverflow(Abutton.first, lcm / Bbutton.first);
        isIntegerOverflow(Abutton.second, lcm / Bbutton.second);

        TokenInt newAbuttonX {Abutton.first * lcm / Bbutton.first};
        TokenInt newAbuttonY {Abutton.second * lcm / Bbutton.second};

        isIntegerOverflow(prize.first, lcm / Bbutton.first);
        isIntegerOverflow(prize.second, lcm / Bbutton.second);

        TokenInt newPrizeX {prize.first * lcm / Bbutton.first};
        TokenInt newPrizeY {prize.second * lcm / Bbutton.second};
        
        // solve for Apresses
        // another check if there is a solution to the equations
        if ((newPrizeY - newPrizeX) % (newAbuttonY - newAbuttonX) != 0)
            return 0;

        TokenInt Apresses {(newPrizeY - newPrizeX) / (newAbuttonY - newAbuttonX)};
        
        // next solve for Bpresses, using the first equation
        // another check if there is a solution to the equations
        if ((prize.first - Abutton.first * Apresses) % Bbutton.first != 0)
            return 0;

        TokenInt Bpresses {(prize.first - Abutton.first * Apresses) / Bbutton.first};

        // finally we can return the number of tokens
        // but first check for overflow
        isIntegerOverflow(Apresses, A);
        isIntegerOverflow(Bpresses, B);

        // backup to make sure the calculation was done right
        confirmCalculation(Apresses, Bpresses, Abutton, Bbutton, prize);
        
        return A * Apresses + B * Bpresses;
    }

    template <TokenInt A, TokenInt B>
    TokenInt getLeastTokensForWinnableMachines(const Machines& machines)
    {
        TokenInt tokens {0};
        for (const Machine& machine : machines)
        {
            tokens += solveForButtonCombinations<A, B>(machine);
        }
        return tokens;
    }

    template <std::size_t N, TokenInt A, TokenInt B>
    TokenInt parseAndGetFewestTokensToWin(const std::array<std::string_view, N>& lines)
    {
        Machines machines {getMachineConfigurations<N>(lines)};
        return getLeastTokensForWinnableMachines<A, B>(machines);
    }
}

#endif // AOC13B_H
