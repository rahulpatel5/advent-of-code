#ifndef AOC13A_H
#define AOC13A_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include <cassert>

/*
parse input to get button moves and prize location targets
set cost for x and y buttons
find combinations for different button presses
get max number of presses of each button (how many fit into total) to give limit for iterating
find combinations that reach prize target, and keep the smallest token cost
stop iterating for combinations that go beyond the target
    [for better efficiency?] do check to stop when combination will definitely fail
count tokens for successful prizes and total for all winnable prizes
*/

namespace aoc13a
{
    using Machine = std::vector<std::pair<int, int>>;
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
                int x {std::stoi(std::string(line.substr(firstPlus + 1, comma - (firstPlus + 1))))};

                size_t secondPlus {line.find('+', firstPlus + 1)};
                int y {std::stoi(std::string(line.substr(secondPlus + 1)))};

                if (line.find(buttonA) != std::string_view::npos)
                    Abuttons.push_back({x, y});
                else
                    Bbuttons.push_back({x, y});
            }
            else if (line.find(prizes) != std::string_view::npos)
            {
                size_t firstEqual {line.find('=')};
                size_t comma {line.find(',')};
                int x {std::stoi(std::string(line.substr(firstEqual + 1, comma - (firstEqual + 1))))};

                size_t secondEqual {line.find('=', firstEqual + 1)};
                int y {std::stoi(std::string(line.substr(secondEqual + 1)))};

                prizeLocations.push_back({x, y});
            }
        }
        assert((Abuttons.size() == Bbuttons.size() && Bbuttons.size() == prizeLocations.size()) && "The number of A buttons, B buttons, and prizes are not the same.");

        Machines machines {};
        for (size_t i{0}; i < Abuttons.size(); ++i)
            machines.push_back({Abuttons[i], Bbuttons[i], prizeLocations[i]});
        return machines;
    }

    int getMaxLoops(std::pair<int, int> button, std::pair<int, int> prize)
    {
        int maxXLoops {prize.first / button.first};
        int maxYLoops {prize.second / button.second};
        return (maxXLoops < maxYLoops) ? maxXLoops : maxYLoops;
    }

    std::pair<int, int> getPresses(std::pair<int, int> Abutton, int Apresses, std::pair<int, int> Bbutton, int Bpresses, std::pair<int, int> prize)
    {
        // guard clauses
        if (Apresses == 0 && Bpresses == 0)
            return {0, 0};
        if (Apresses == 0 && ((prize.first % (Bbutton.first * Bpresses) != 0) && (prize.second % (Bbutton.second * Bpresses) != 0)))
            return {0, 0};
        if (Bpresses == 0 && ((prize.first % (Abutton.first * Apresses) != 0) && (prize.second % (Abutton.second * Apresses) != 0)))
            return {0, 0};

        int totalPresses {1};
        int x {0};
        int y {0};
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

    template <int A, int B>
    int tryButtonCombinations(const Machine& machine)
    {
        int tokens {0};
        std::pair<int, int> Abutton {machine[0]};
        std::pair<int, int> Bbutton {machine[1]};
        std::pair<int, int> prize {machine[2]};

        constexpr int maxPresses {100}; // stated in puzzle
        // I thought the following would make things more efficient
        // but it's actually wrong
        // int maxALoops {getMaxLoops(Abutton, prize)};
        // int maxBLoops {getMaxLoops(Bbutton, prize)};
        // maxALoops = ((maxALoops < maxPresses) ? maxALoops : maxPresses);
        // maxBLoops = ((maxBLoops < maxPresses) ? maxBLoops : maxPresses);

        std::pair<int, int> empty {0, 0};
        // try different combinations of button presses
        for (int a{0}; a < maxPresses; ++a)
        {
            for (int b{0}; b < maxPresses; ++b)
            {
                if (a == 0 && b > 1)
                    break;
                if (a > 1 && b == 0)
                    continue;
                
                std::pair<int, int> presses {getPresses(Abutton, a, Bbutton, b, prize)};
                if ((presses != empty) && (tokens == 0 || ((A * presses.first + B * presses.second) < tokens)))
                {
                    tokens = (A * presses.first + B * presses.second);
                    break;
                }
            }
        }
        
        return tokens;
    }

    template <int A, int B>
    int getLeastTokensForWinnableMachines(const Machines& machines)
    {
        int tokens {0};
        for (const Machine& machine : machines)
            tokens += tryButtonCombinations<A, B>(machine);
        return tokens;
    }

    template <std::size_t N, int A, int B>
    int parseAndGetFewestTokensToWin(const std::array<std::string_view, N>& lines)
    {
        Machines machines {getMachineConfigurations<N>(lines)};
        return getLeastTokensForWinnableMachines<A, B>(machines);
    }
}

#endif // AOC13A_H
