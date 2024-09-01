#ifndef AOC4A_H
#define AOC4A_H

#include "external/MD5/src/md5.h"
#include <iostream>
#include <cstring>
#include <sstream>

namespace aoc4a
{
    std::string getMD5(const std::string& input)
    {
        md5::md5_t hash{};
        hash.process(input.c_str(), input.size());
        hash.finish();

        char str[MD5_STRING_SIZE];
        hash.get_string(str);
        return str;
    }

    bool doesMD5StartWithZeroes(std::string_view hash, int targetZeroes)
    {
        for (auto i{0}; i < targetZeroes; ++i)
        {
            if (hash[i] != '0')
                return false;
        }
        return true;
    }

    void getFirstValidKey(const std::string& input, int targetZeroes, int maxIterations)
    {
        std::ostringstream oss{};
        for (auto i{1}; i < maxIterations; ++i)
        {
            oss.str("");
            oss.clear();
            oss << input << i;
            std::string iteration {oss.str()};
            
            std::string hash {getMD5(iteration)};
            if (doesMD5StartWithZeroes(hash, targetZeroes))
            {
                std::cout << "First valid number is: " << i << '\n';
                return;
            }
        }

        std::cout << "Did not find a valid hash after " << maxIterations << " iterations.\n";
    }
}

#endif
