#ifndef AOC5B_H
#define AOC5B_H

#include <string_view>
#include <array>
#include <vector>
#include <sstream>

/*
read order rules
read page updates
iterate each set of page updates
for a page update set, iterate rules to check if page update is valid
discard valid updates and collect non-valid ones
run through non-valid updates and correct them
(note: if making code more efficient, try to combine the above two steps)
check that corrected non-valid updates now work
sum middle figures for corrected non-valid updates
*/

namespace aoc5b
{
    template <std::size_t N>
    std::vector<std::pair<int, int>> readOrderRules(std::array<std::string_view, N> lines)
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
    std::vector<std::vector<int>> readPageUpdates(std::array<std::string_view, N> lines)
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

    bool isUpdateNotValid(std::vector<int> update, std::vector<std::pair<int, int>> rules)
    {
        for (const std::pair<int, int>& rule : rules)
        {
            bool activeSearch {false};
            for (int number : update)
            {
                if (rule.second == number)
                    activeSearch = true;
                if ((activeSearch) && (rule.first == number))
                    return true;
            }
        }
        return false;
    }

    int getMiddleNumber(std::vector<int> update)
    {
        size_t middle {(update.size() - 1) / 2};
        return update.at(middle);
    }

    std::vector<int> fixNonValidUpdate(std::vector<int> nonValidUpdate, std::vector<std::pair<int, int>> rules)
    {
        std::vector<int> fixedUpdate {nonValidUpdate};
        for (const std::pair<int, int>& rule : rules)
        {
            bool activeSearch {false};
            std::size_t laterNum {};
            for (std::size_t i{0}; i < fixedUpdate.size(); ++i)
            {
                if (rule.second == fixedUpdate[i])
                {
                    activeSearch = true;
                    laterNum = i;
                }
                if ((activeSearch) && (rule.first == fixedUpdate[i]))
                {
                    fixedUpdate[i] = nonValidUpdate[laterNum];
                    fixedUpdate[laterNum] = nonValidUpdate[i];
                    return fixNonValidUpdate(fixedUpdate, rules);
                }
            }
        }
        return fixedUpdate;
    }

    int applyRulesToUpdates(std::vector<std::pair<int, int>> rules, std::vector<std::vector<int>> updates)
    {
        int sum {0};

        for (std::size_t i{0}; i < updates.size(); ++i)
        {
            if (isUpdateNotValid(updates[i], rules))
            {
                std::vector<int> fixedUpdate {fixNonValidUpdate(updates[i], rules)};
                sum += getMiddleNumber(fixedUpdate);
            }
        }

        return sum;
    }

    template <std::size_t N>
    int parseAndSumMiddlePages(std::array<std::string_view, N> lines)
    {
        std::vector<std::pair<int, int>> orderRules {readOrderRules<N>(lines)};
        std::vector<std::vector<int>> pageUpdates {readPageUpdates<N>(lines)};

        return applyRulesToUpdates(orderRules, pageUpdates);
    }
}

#endif // AOC5B_H
