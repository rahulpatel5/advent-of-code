#ifndef AOC13B_H
#define AOC13B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>

#include <iostream>

/*
re-use parts of part 1 solution
logic may be to identify where a set of numbers add up to e.g. 100,000
or the power of 10 above the target x or y value
then multiply that to get to the 10'000'000'000'000?
but this doesn't make sense as test machines 1 and 3 were winnable in part 1
but now test machines 2 and 4 are winnable
this may mean that a higher power of 10 is a multiple of the solution

what happens if I just do standard division, like the part 1 solution?
*/

namespace constants
{
    constexpr long long ERROR {10'000'000'000'000};
    // assume that all prizes are at most in the 10'000s
    constexpr long long LOWEST_DIVISOR {100'000};
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
            }
        }
        assert((Abuttons.size() == Bbuttons.size() && Bbuttons.size() == prizeLocations.size()) && "The number of A buttons, B buttons, and prizes are not the same.");

        Machines machines {};
        for (size_t i{0}; i < Abuttons.size(); ++i)
            machines.push_back({Abuttons[i], Bbuttons[i], prizeLocations[i]});
        return machines;
    }

    TokenInt getMaxLoops(PairInt button, PairInt prize)
    {
        TokenInt maxXLoops {prize.first / button.first};
        TokenInt maxYLoops {prize.second / button.second};
        return (maxXLoops < maxYLoops) ? maxXLoops : maxYLoops;
    }

    PairInt getPresses(PairInt Abutton, TokenInt Apresses, PairInt Bbutton, TokenInt Bpresses, PairInt prize)
    {
        // guard clauses
        if (Apresses == 0 && Bpresses == 0)
            return {0, 0};
        if (Apresses == 0 && ((prize.first % (Bbutton.first * Bpresses) != 0) && (prize.second % (Bbutton.second * Bpresses) != 0)))
            return {0, 0};
        if (Bpresses == 0 && ((prize.first % (Abutton.first * Apresses) != 0) && (prize.second % (Abutton.second * Apresses) != 0)))
            return {0, 0};

        TokenInt totalPresses {1};
        TokenInt x {0};
        TokenInt y {0};
        while (x < prize.first && y < prize.second)
        {
            x += Abutton.first * Apresses + Bbutton.first * Bpresses;
            y += Abutton.second * Apresses + Bbutton.second * Bpresses;
            if (x == prize.first && y == prize.second)
                return {Apresses * totalPresses, Bpresses * totalPresses};
            ++totalPresses;
        }
        return {0, 0};
    }

    TokenInt findLowest10sDivisor(PairInt Abutton, TokenInt Apresses, PairInt Bbutton, TokenInt Bpresses)
    {
        if (Apresses == 0 && Bpresses == 0)
            return 0;
        for (TokenInt i{constants::LOWEST_DIVISOR}; i <= constants::ERROR; i *= 10)
        {
            if ((i % (Abutton.first * Apresses + Bbutton.first * Bpresses) == 0) && (i % (Abutton.second * Apresses + Bbutton.second * Bpresses) == 0))
            {
                std::cout << "have succeeded\n";
                return i;
            }
        }
        return 0;
    }

    template <TokenInt A, TokenInt B>
    TokenInt tryButtonCombinations(const Machine& machine)
    {
        TokenInt tokens {0};
        PairInt Abutton {machine[0]};
        PairInt Bbutton {machine[1]};
        PairInt prize {machine[2]};

        // no longer a maximum number of presses
        // set to increasing values while testing
        // think most of this is handled with higher orders of 10
        constexpr int maxPresses {10'000};

        PairInt empty {0, 0};
        // try different combinations of button presses
        for (TokenInt a{0}; a < maxPresses; ++a)
        {
            for (TokenInt b{0}; b < maxPresses; ++b)
            {
                // if (a == maxPresses - 1 && b == maxPresses - 1)
                //     std::cout << "end of another\n";
                if (a == 0 && b > 1)
                    break;
                if (a > 1 && b == 0)
                    continue;
                
                // find lowest order of 10 divisor (starting from 10**5)
                TokenInt lowest10sDivisor {findLowest10sDivisor(Abutton, a, Bbutton, b)};
                if (lowest10sDivisor == 0)
                    continue;
                
                TokenInt remainingX {prize.first % lowest10sDivisor};
                TokenInt remainingY {prize.second % lowest10sDivisor};
                // even if multiple of 10, skip if the rest doesn't divide
                if ((remainingX % (Abutton.first * a + Bbutton.first * b) != 0) || (remainingY % (Abutton.second * a + Bbutton.second * b) != 0))
                    continue;

                std::cout << lowest10sDivisor << '\n';
                std::cout << prize.first << '\n';
                std::cout << '\t' << Abutton.first << '\n';
                std::cout << '\t' << Bbutton.first << '\n';
                std::cout << prize.second << '\n';
                std::cout << '\t' << Abutton.second << '\n';
                std::cout << '\t' << Bbutton.second << '\n';
                
                PairInt presses {getPresses(Abutton, a, Bbutton, b, prize)};
                if ((presses != empty) && (tokens == 0 || ((A * presses.first + B * presses.second) < tokens)))
                {
                    tokens = (A * presses.first + B * presses.second);
                    break;
                }
            }
        }
        return tokens;
    }

    template <TokenInt A, TokenInt B>
    int getLeastTokensForWinnableMachines(const Machines& machines)
    {
        int tokens {0};
        for (const Machine& machine : machines)
        {
            tokens += tryButtonCombinations<A, B>(machine);
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
