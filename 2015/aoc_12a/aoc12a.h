#ifndef AOC12A_H
#define AOC12A_H

#include <string_view>
#include <string>

namespace aoc12a
{
    using SumInt = long long;

    SumInt jsonSum(std::string_view str)
    {
        SumInt sum {0};

        std::string coll {};
        for (char c : str)
        {
            if ((coll.size() == 0 && c == '-') || ((c - '0') >= 0 && (c - '0') <= 9))
            {
                coll += c;
            }
            else if (coll.size() > 0 && c != '-' && !((c - '0') >= 0 && (c - '0') <= 9))
            {
                sum += std::stoi(coll);
                coll = "";
            }
        }

        return sum;
    }
}

#endif // AOC12A_H
