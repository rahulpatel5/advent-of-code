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

        // use next_permutation to find each iteration
        std::vector<char> outerVector {concatAndPluses};
        for (size_t i{0}; i < concatAndPluses.size(); ++i)
        {
            // need to use ( as we want a character with an earlier ASCII code than + or *
            // the permutations (next_permutation) don't work unless starting in ascending order
            outerVector[i] = '(';
            permutations.push_back(outerVector);

            std::vector<char> copyVector {outerVector};
            while (std::next_permutation(copyVector.begin(), copyVector.end()))
            {
                permutations.push_back(copyVector);
            }

            std::vector<char> innerVector {outerVector};
            if (i < concatAndPluses.size() - 1)
            {
                for (size_t j{i + 1}; j < concatAndPluses.size(); ++j)
                {
                    innerVector[j] = '*';
                    permutations.push_back(innerVector);

                    std::vector<char> copycopyVector {innerVector};
                    while (std::next_permutation(copycopyVector.begin(), copycopyVector.end()))
                    {
                        permutations.push_back(copycopyVector);
                    }
                }
            }
        }

        return permutations;
    }
}

#endif // PERMUTATION_7B_H
