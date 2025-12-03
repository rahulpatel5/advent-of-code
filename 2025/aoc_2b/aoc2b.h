#ifndef AOC2B_H
#define AOC2B_H

#include <string_view>
#include <string>
#include <set>
#include <stdexcept>

/*
FOR each range:
    FOR any numbers up to half the number of digits:
        IF the number repeated (n times) is in the range:
            ADD to the sum

RETURN sum
*/

namespace aoc2b
{
    using InvalidInt = long long;
    using Line = std::string_view;
    using Range = std::string;
    using NumberStr = std::string;
    using Number = long long;
    using UniqueNumbers = std::set<Number>;

    InvalidInt sumInvalidIDs(const Line& line)
    {
        InvalidInt sumInvalidInt {0};

        Range ranges { std::string(line) };
        // set to 0 so that we get the first range
        size_t comma {0};

        while (comma != std::string::npos)
        {
            size_t hyphen { ranges.find('-', comma) };
            NumberStr firstNumStr {};
            if (comma == 0)
                firstNumStr = ranges.substr(comma, hyphen - comma);
            else
                firstNumStr = ranges.substr(comma + 1, hyphen - (comma + 1));
            size_t nextComma { ranges.find(',', comma + 1) };
            NumberStr secondNumStr { ranges.substr(hyphen + 1, nextComma - (hyphen + 1)) };

            Number firstNum { std::stoll(firstNumStr) };
            Number secondNum { std::stoll(secondNumStr) };

            // if first number is <10, set to 10
            if (firstNum < 10)
            {
                firstNum = 10;
                firstNumStr = "10";
            }

            // as for last part, check the numbers aren't >1 order apart
            if (secondNumStr.size() - firstNumStr.size() > 1)
                throw std::out_of_range("Numbers are more than one order of magnitude apart. The current code isn't set up to handle this.\n");

            size_t halfFirstNumSizeStr { firstNumStr.size() / 2 };
            size_t halfSecondNumSizeStr { secondNumStr.size() / 2 };
            // if first number has even digits and second is odd
            // then want to add one to halfSecondNumSizeStr
            if (firstNumStr.size() % 2 == 0 && secondNumStr.size() % 2 != 0)
                ++halfSecondNumSizeStr;
            Number halfFirstNum { std::stoll(firstNumStr.substr(0, halfFirstNumSizeStr)) };
            Number halfSecondNum { std::stoll(secondNumStr.substr(0, halfSecondNumSizeStr)) };

            UniqueNumbers unique {};
            for (Number i {halfFirstNum}; i <= halfSecondNum; ++i)
            {
                // for each number, i, check each digit
                NumberStr iStr { std::to_string(i) };
                for (size_t j{1}; j <= iStr.size(); ++j)
                {
                    // skip if number of digits won't fit when repeated
                    if (firstNumStr.size() % j != 0 && secondNumStr.size() % j != 0)
                        continue;
                    
                    // construct for the first number, if it fits
                    if (firstNumStr.size() % j == 0)
                    {
                        NumberStr numStrToCheck {};
                        for (size_t k{0}; k < firstNumStr.size() / j; ++k)
                        {
                            numStrToCheck += iStr.substr(0, j);
                        }
                        Number numToCheck { std::stoll(numStrToCheck) };
                        
                        // check if this number is in the range
                        // if so, add to our sum
                        if (numToCheck >= firstNum && numToCheck <= secondNum && unique.find(numToCheck) == unique.end())
                        {
                            sumInvalidInt += numToCheck;
                            unique.insert(numToCheck);
                        }
                    }

                    // construct for second number, if it fits
                    // and if second number isn't the same size
                    if (secondNumStr.size() != firstNumStr.size() && secondNumStr.size() % j == 0)
                    {
                        NumberStr numStrToCheck2 {};
                        for (size_t l{0}; l < secondNumStr.size() / j; ++l)
                        {
                            numStrToCheck2 += iStr.substr(0, j);
                        }
                        Number numToCheck2 { std::stoll(numStrToCheck2) };

                        if (numToCheck2 >= firstNum && numToCheck2 <= secondNum && unique.find(numToCheck2) == unique.end())
                        {
                            sumInvalidInt += numToCheck2;
                            unique.insert(numToCheck2);
                        }
                    }
                }
            }

            comma = nextComma;
        }

        return sumInvalidInt;
    }
}

#endif // AOC2B_H
