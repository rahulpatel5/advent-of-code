#ifndef AOC5B_H
#define AOC5B_H

#include <string_view>
#include <array>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <cassert>

/*
re-use faster part 1 solution
flip logic to get incorrectly ordered updates
correct ordering for those and get middle page
*/

namespace aoc5b
{
    using Page = int;
    using UniquePages = std::set<Page>;
    using Rules = std::map<Page, UniquePages>;
    using Updates = std::vector<Page>;
    using UpdateLists = std::vector<Updates>;

    template <std::size_t N>
    void readRulesAndUpdates(Rules& rules, UpdateLists& updates, const std::array<std::string_view, N>& lines)
    {
        for (const std::string_view& line : lines)
        {
            // collect rules if there's a '|' in the line
            if (line.find('|') != std::string_view::npos)
            {
                size_t pipe {line.find('|')};
                int num1 {std::stoi(std::string(line.substr(0, pipe)))};
                int num2 {std::stoi(std::string(line.substr(pipe + 1)))};
                rules[num1].insert(num2);
            }

            // collect updates if there's a ',' in the line
            else if (line.find(',') != std::string_view::npos)
            {
                size_t index {0};
                size_t comma {};
                Updates inner {};
                while (comma != std::string_view::npos)
                {
                    comma = line.find(',', index);
                    int num {std::stoi(std::string(line.substr(index, comma)))};
                    inner.push_back(num);
                    index = comma + 1;
                }
                updates.push_back(inner);
            }
        }
    }

    Updates correctPageOrder(const Updates& updates, const Rules& rules)
    {
        Updates corrected {updates};
        // we want to apply a sorting algorithm
        for (size_t start{0}; start < corrected.size(); ++start)
        {
            size_t correctIndex {start};
            for (size_t next{start + 1}; next < corrected.size(); ++next)
            {
                if (rules.at(corrected.at(next)).find(corrected.at(correctIndex)) != rules.at(corrected.at(next)).end())
                {
                    correctIndex = next;
                }
            }
            std::swap(corrected[start], corrected[correctIndex]);
        }
        return corrected;
    }

    int correctOrderAndGetMiddleNumber(const Updates& updates, const Rules& rules)
    {
        assert(updates.size() % 2 != 0 && "Error: got an unexpected even number of page numbers while looking for middle page.\n");

        Updates corrected {correctPageOrder(updates, rules)};

        size_t middleIndex {(corrected.size() - 1) / 2};
        return corrected.at(middleIndex);
    }

    int findCorrectUpdatesAndGetMiddleNumber(const Rules& rules, const UpdateLists& updateLists)
    {
        int sum {0};

        for (const Updates& updates : updateLists)
        {
            UniquePages previous {};
            bool isCorrectOrder {true};
            for (const Page& page : updates)
            {
                // check if any previous elements are in rules for current
                // rules for current element are those that should be after
                // if they're before then the order is incorrect
                UniquePages intersection;
                std::set_intersection(previous.begin(), previous.end(), 
                                      rules.at(page).begin(), rules.at(page).end(),
                                      std::inserter(intersection, intersection.begin()));
                if (!intersection.empty())
                {
                    isCorrectOrder = false;
                    break;
                }

                previous.insert(page);
            }

            if (!isCorrectOrder)
            {
                sum += correctOrderAndGetMiddleNumber(updates, rules);
            }
        }

        return sum;
    }

    template <std::size_t N>
    int parseAndSumMiddlePages(const std::array<std::string_view, N>& lines)
    {
        Rules orderRules {};
        UpdateLists pageUpdates {};
        readRulesAndUpdates<N>(orderRules, pageUpdates, lines);

        return findCorrectUpdatesAndGetMiddleNumber(orderRules, pageUpdates);
    }
}

#endif // AOC5B_H
