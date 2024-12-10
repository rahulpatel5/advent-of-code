#ifndef PERMUTATION_7B_H
#define PERMUTATION_7B_H

#include <vector>
#include <algorithm>

namespace permutation_7b
{
    std::vector<std::vector<char>> generateWithConcat(std::vector<int> arr)
    {
        std::vector<std::vector<char>> permutations {};

        std::vector<char> concatAndPluses {};
        for (size_t i{1}; i < arr.size(); ++i)
            concatAndPluses.push_back('+');
        concatAndPluses[0] = '|';
        permutations.push_back(concatAndPluses);

        // use next_permutation to find each iteration
        std::vector<char> copyVector {concatAndPluses};
        for (size_t i{1}; i < concatAndPluses.size(); ++i)
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

#endif // PERMUTATION_7B_H
