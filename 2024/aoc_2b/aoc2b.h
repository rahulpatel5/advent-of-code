#ifndef AOC2B_H
#define AOC2B_H

#include <vector>
#include <string>
#include <sstream>

namespace aoc2b
{
    template <typename T>
    std::vector<int> readReport(const T& line)
    {
        std::vector<int> report {};
        std::istringstream testLine {std::string(line)};
        std::string segment;
        while (std::getline(testLine, segment, ' '))
        {
            report.push_back(std::stoi(segment));
        }
        return report;
    }

    std::vector<int> getDifferences(std::vector<int>& report)
    {
        std::vector<int> differences {};
        for (auto i{1}; i < report.size(); ++i)
        {
            int difference {report[i] - report[i - 1]};
            differences.push_back(difference);
        }
        return differences;
    }

    std::vector<int> changeReport(std::vector<int>& report, int index)
    {
        if (index < 0)
            return report;
        
        std::vector<int> newReport {};
        for (auto i{0}; i < report.size(); ++i)
        {
            if (i == index)
                continue;
            newReport.push_back(report[i]);
        }
        return newReport;
    }

    int validIncreaseOrDecrease(std::vector<int>& differences)
    {
        if (differences[0] == 0)
            return false;
        else if (differences[0] < 0)
        {
            for (auto i{0}; i < differences.size(); ++i)
            {
                if ((differences[i] > -1) | (differences[i] < -3))
                    return false;
            }
        }
        else
        {
            for (auto j{0}; j < differences.size(); ++j)
            {
                if ((differences[j] < 1) | (differences[j] > 3))
                    return false;
            }
        }

        return true;
    }

    bool isSafe(std::vector<int>& report)
    {
        std::vector<int> differences {getDifferences(report)};
        if (validIncreaseOrDecrease(differences))
            return true;
        for (auto i{0}; i < report.size(); ++i)
        {
            std::vector<int> newReport {changeReport(report, i)};
            std::vector<int> newDifferences {getDifferences(newReport)};
            if (validIncreaseOrDecrease(newDifferences))
                return true;
        }

        return false;
    }

    template <typename T>
    int parseInputAndCountSafe(const T& lines)
    {
        int safeCount {0};

        // parse input and extract numbers
        for (const auto& line : lines)
        {
            std::vector<int> report {readReport(line)};
            if (isSafe(report))
                ++safeCount;
        }

        return safeCount;
    }
}

#endif // AOC2B_H
