#ifndef AOC25A_H
#define AOC25A_H

#include <array>
#include <string_view>
#include <string>

/*
GET row and column position from message
GET total calculations need to reach code
    USE sum of (1 to n), i.e. n * (n + 1) / 2
    ADD extra rows (subtract 1)
CALCULATE each step, i.e.
    MULTIPLY by multiplier
    GET remainder from dividing by divisor
RETURN code

is there a way to do this with fewer calculations?
*/

namespace aoc25a
{
    using CodeInt = long long;

    using Message = std::string_view;

    using Row = int;
    using Col = int;
    using Position = std::pair<Row, Col>;
    using CodeIndex = long long;

    Position getPosition(Message message)
    {
        std::string input {message};

        size_t rowRow {input.find("row")};
        size_t postRowComma {input.find(',', rowRow)};
        Row row {std::stoi(input.substr(rowRow + 4, postRowComma - (rowRow + 4)))};

        size_t lastSpace {input.find_last_of(' ')};
        size_t lastStop {input.find_last_of('.')};
        Col col {std::stoi(input.substr(lastSpace + 1, lastStop - (lastSpace + 1)))};

        return std::make_pair(row, col);
    }

    // + (codePosition.first - 1) * codePosition.second 
    CodeIndex getCodeIndex(Position codePosition)
    {
        return (codePosition.second * (codePosition.second + 1) / 2) + ((codePosition.first - 1) * (codePosition.second + codePosition.second + (codePosition.first - 2)) / 2);
    }

    CodeInt findCode(CodeInt firstCode, CodeIndex index, int multiplier, int divisor)
    {
        CodeInt code {firstCode};
        for (CodeIndex i{2}; i <= index; ++i)
        {
            code = (code * multiplier) % divisor;
        }
        return code;
    }

    CodeInt getCode(Message message, CodeInt firstCode, int multiplier, int divisor)
    {
        Position codePosition {getPosition(message)};
        CodeIndex index {getCodeIndex(codePosition)};

        return findCode(firstCode, index, multiplier, divisor);
    }
}

#endif // AOC25A_H
