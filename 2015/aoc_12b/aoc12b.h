#ifndef AOC12B_H
#define AOC12B_H

#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

/*
parse json input
replace objects and lists with their sum
take sum of properties in aggregated object
*/

namespace aoc12b
{
    using SumInt = long long;
    using Scores = std::vector<SumInt>;
    using SumStr = std::string;

    SumInt safeStoi(const std::string& str)
    {
        try {
            return std::stoi(str);
        } catch (const std::exception&) {
            return 0;
        }
    }

    SumStr processList(const std::string& str)
    {
        size_t pos {0};
        SumInt sum {0};
        while (pos != std::string::npos)
        {
            size_t comma {str.find(',', pos)};
            sum += safeStoi(str.substr(pos, comma));
            if (comma == std::string::npos)
                pos = comma;
            else
                pos = comma + 1;
        }
        return std::to_string(sum);
    }

    SumStr processObj(const std::string& str)
    {
        size_t pos {0};
        SumInt sum {0};
        bool isRed {false};
        while (pos != std::string::npos)
        {
            size_t colon {str.find(':', pos)};
            size_t comma {str.find(',', colon)};
            sum += safeStoi(str.substr(colon + 1, comma));
            if ("\"red\"" == str.substr(colon + 1, comma - colon - 1))
                isRed = true;
            if (comma == std::string::npos)
                pos = comma;
            else
                pos = comma + 1;
        }
        return isRed ? "0" : std::to_string(sum);
    }

    std::string parseJson(std::string_view original)
    {
        // copy the input so we can remove elements
        std::string str {std::string(original)};

        while (std::count(str.begin(), str.end(), '{') > 1)
        {
            size_t curr {0};

            while (curr != std::string::npos)
            {
                size_t objSt {str.find('{', curr)};
                size_t listSt {str.find('[', curr)};

                if (objSt == std::string::npos && listSt == std::string::npos)
                {
                    curr = std::string::npos;
                }
                else if (objSt < listSt)
                {
                    size_t objEd {str.find('}', objSt)};
                    size_t listEd {str.find(']', objSt)};
                    size_t nextObjSt {str.find('{', objSt + 1)};

                    if (nextObjSt < std::min({listSt, listEd, objEd}))
                    {
                        curr = nextObjSt;
                    }
                    else if (objEd < std::min(listSt, listEd))
                    {
                        SumStr sumStr {processObj(str.substr(objSt + 1, objEd - objSt - 1))};
                        str.erase(objSt, objEd - objSt + 1);
                        str.insert(objSt, sumStr);
                        curr = objSt + sumStr.size();
                    }
                    else if (listSt < std::min(objEd, listEd))
                    {
                        curr = listSt;
                    }
                    else
                    {
                        throw std::out_of_range("Didn't expect ']' to be first after '{'.\n");
                    }
                }
                else if (listSt < objSt)
                {
                    size_t listEd {str.find(']', listSt)};
                    size_t objEd {str.find('}', listSt)};
                    size_t nextListSt {str.find('[', listSt + 1)};

                    if (nextListSt < std::min({objSt, objEd, listEd}))
                    {
                        curr = nextListSt;
                    }
                    else if (listEd < std::min(objSt, objEd))
                    {
                        SumStr sumStr {processList(str.substr(listSt + 1, listEd - listSt - 1))};
                        str.erase(listSt, listEd - listSt + 1);
                        str.insert(listSt, sumStr);
                        curr = listSt + sumStr.size();
                    }
                    else if (objSt < std::min(objEd, listEd))
                    {
                        curr = objSt;
                    }
                    else
                    {
                        throw std::out_of_range("Didn't expect '}' to be first after '['.\n");
                    }
                }
            }
        }

        return str;
    }

    SumInt jsonSum(std::string_view str)
    {
        std::string procStr {parseJson(str)};

        // remove the brackets
        size_t start {procStr.find('{')};
        size_t end {procStr.find('}')};
        std::string finalStr {procStr.substr(start + 1, end - start - 1)};

        SumInt sum {0};
        size_t curr {0};
        while (curr != std::string::npos)
        {
            size_t colon {finalStr.find(':', curr)};
            size_t comma {finalStr.find(',', curr)};
            sum += safeStoi(finalStr.substr(colon + 1, comma - colon));
            if (comma == std::string::npos)
                curr = comma;
            else
                curr = comma + 1;
        }

        return sum;
    }
}

#endif // AOC12B_H
