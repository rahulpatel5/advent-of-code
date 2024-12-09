#ifndef PERMUTATION_H
#define PERMUTATION_H

#include <vector>
#include <algorithm>

namespace permutation
{
    void recursion(std::vector<std::vector<char>>& permutations, const std::vector<char>& newVector, size_t index, int replacements, int count)
    {
        ++count;
        for (size_t i{index}; i < newVector.size(); ++i)
        {
            std::vector<char> copyVector {newVector};
            copyVector[i] = '*';
            if (count < replacements)
                recursion(permutations, copyVector, ++i, replacements, count);
            else
            {
                permutations.push_back(copyVector);
            }
        }
    }

    std::vector<std::vector<char>> generate(std::vector<int> arr)
    {
        std::vector<std::vector<char>> permutations {};

        std::vector<char> allPluses {};
        for (size_t i{1}; i < arr.size(); ++i)
            allPluses.push_back('+');
        permutations.push_back(allPluses);

        // // recursion not working. Figure out how to get this to work
        // for (int i{1}; i <= allPluses.size(); ++i)
        // {
        //     int count {0};
        //     int index {0};
        //     recursion(permutations, allPluses, index, i, count);
        // }
        
        // use next_permutation to find each iteration
        // need to check if putting '*' or '+' first works
        // '*' seems to have a smaller ASCII number
        std::vector<char> copyVector {allPluses};
        for (size_t i{0}; i < allPluses.size(); ++i)
        {
            copyVector[i] = '*';
            permutations.push_back(copyVector);
            while (std::next_permutation(copyVector.begin(), copyVector.end()))
            {
                permutations.push_back(copyVector);
            }
        }

        return permutations;
    }
}

#endif // PERMUTATION_H
