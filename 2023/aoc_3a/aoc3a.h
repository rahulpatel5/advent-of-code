#ifndef AOC3A_H
#define AOC3A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

/*
READ the schematic

FOR each number found in the schematic:
    IDENTIFY if any symbol (not digit or period) is adjacent
    IF adjacent (diagonal allowed):
        ADD to sum of part numbers
    ELSE:
        SKIP to next number

RETURN sum of part numbers
*/

namespace aoc3a
{
    using PartInt = long long;

    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;
    using DigitChain = std::vector<int>;

    using Col = size_t;
    using Columns = std::vector<Col>;
    using Row = size_t;
    using SymbolMap = std::map<Row, Columns>;

    template <std::size_t N>
    bool isDigit(const Lines<N>& map, size_t row, size_t col)
    {
        return (map.at(row).at(col) >= '0' && map.at(row).at(col) <= '9');
    }

    template <std::size_t N>
    SymbolMap getSymbols(const Lines<N>& map)
    {
        SymbolMap symbols {};
        for (size_t row{0}; row < map.size(); ++row)
        {
            for (size_t col{0}; col < map.at(row).size(); ++col)
            {
                if ((map.at(row).at(col) != '.') && !isDigit<N>(map, row, col))
                    symbols[row].push_back(col);
            }
        }
        return symbols;
    }

    template <std::size_t N>
    bool isPartNumber(const Lines<N>& map, size_t row, const DigitChain& chain, const SymbolMap& symbols)
    {
        // look around the positions where the digits are
        for (int rowShift{-1}; rowShift <= 1; ++rowShift)
        {
            if (symbols.find(row + rowShift) == symbols.end())
                continue;
            
            for (int col{chain.front() - 1}; col <= chain.back() + 1; ++col)
            {
                if (std::find(symbols.at(row + rowShift).begin(), symbols.at(row + rowShift).end(), col) != symbols.at(row + rowShift).end())
                    return true;
            }
        }

        return false;
    }

    template <std::size_t L>
    PartInt getSumOfPartNumbers(const Lines<L>& lines)
    {
        SymbolMap symbols {getSymbols<L>(lines)};

        PartInt sumPartNum {0};

        size_t rowIndex {0};
        size_t colIndex {0};
        while (rowIndex < lines.size())
        {
            while (colIndex < lines.at(rowIndex).size())
            {
                if (!isDigit<L>(lines, rowIndex, colIndex))
                {
                    ++colIndex;
                    continue;
                }

                DigitChain chain {};
                chain.push_back(colIndex);
                ++colIndex;
                while ((colIndex < lines.at(rowIndex).size()) && isDigit<L>(lines, rowIndex, colIndex))
                {
                    chain.push_back(colIndex);
                    ++colIndex;
                }

                if (isPartNumber<L>(lines, rowIndex, chain, symbols))
                {
                    std::string partStr {lines.at(rowIndex).at(chain.at(0))};
                    for (size_t i{1}; i < chain.size(); ++i)
                    {
                        partStr += lines.at(rowIndex).at(chain.at(i));
                    }
                    PartInt partNum {std::stoll(partStr)};
                    sumPartNum += partNum;
                }
            }
            ++rowIndex;
            colIndex = 0;
        }

        return sumPartNum;
    }
}

#endif // AOC3A_H
