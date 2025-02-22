#ifndef AOC5A_H
#define AOC5A_H

#include <string_view>
#include <array>
#include <vector>
#include <sstream>

namespace aoc5a
{
    template <std::size_t N>
    std::vector<std::pair<int, int>> readOrderRules(const std::array<std::string_view, N>& lines)
    {
        std::vector<std::pair<int, int>> rules {};
        
        for (std::string_view line : lines)
        {
            if (line.find('|') == std::string_view::npos)
                continue;

            size_t pipe {line.find('|')};
            int num1 {std::stoi(std::string(line.substr(0, pipe)))};
            int num2 {std::stoi(std::string(line.substr(pipe + 1)))};
            rules.push_back({num1, num2});
        }

        return rules;
    }

    template <std::size_t N>
    std::vector<std::vector<int>> readPageUpdates(const std::array<std::string_view, N>& lines)
    {
        std::vector<std::vector<int>> updates {};

        for (std::string_view line : lines)
        {
            if (line.find(',') == std::string_view::npos)
                continue;
            
            size_t start {0};
            size_t end {0};
            std::vector<int> innerVec {};

            while (end != std::string_view::npos)
            {
                end = line.find(',', start);
                int num {std::stoi(std::string(line.substr(start, end - start)))};
                innerVec.push_back(num);
                start = end + 1;
            }

            updates.push_back(innerVec);
        }

        return updates;
    }

    bool isUpdateValid(const std::vector<int>& update, const std::vector<std::pair<int, int>>& rules)
    {
        for (const std::pair<int, int>& rule : rules)
        {
            bool activeSearch {false};
            for (int number : update)
            {
                if (rule.second == number)
                    activeSearch = true;
                if ((activeSearch) && (rule.first == number))
                    return false;
            }
        }
        return true;
    }

    int getMiddleNumber(const std::vector<int>& update)
    {
        size_t middle {(update.size() - 1) / 2};
        return update.at(middle);
    }

    int applyRulesToUpdates(const std::vector<std::pair<int, int>>& rules, const std::vector<std::vector<int>>& updates)
    {
        int sum {0};

        for (std::size_t i{0}; i < updates.size(); ++i)
        {
            if (isUpdateValid(updates[i], rules))
                sum += getMiddleNumber(updates[i]);
        }

        return sum;
    }

    template <std::size_t N>
    int parseAndSumMiddlePages(const std::array<std::string_view, N>& lines)
    {
        std::vector<std::pair<int, int>> orderRules {readOrderRules<N>(lines)};
        std::vector<std::vector<int>> pageUpdates {readPageUpdates<N>(lines)};

        return {applyRulesToUpdates(orderRules, pageUpdates)};
    }
}

#endif // AOC5A_H
