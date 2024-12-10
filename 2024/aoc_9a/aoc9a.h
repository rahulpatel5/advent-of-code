#ifndef AOC9A_H
#define AOC9A_H

#include <string_view>
#include <vector>
#include <string>

/*
process string into vector (because integers can go above single digits)
rearrange vector to move later numbers to fill earlier 'empty' ones
calculate checksum
*/

namespace aoc9a
{
    std::vector<int> getIndividualBlocks(std::string_view input)
    {
        std::vector<int> blocks {};
        int fileID {0};
        int free {-1};

        for (size_t i{0}; i < input.size(); ++i)
        {
            int size {input[i] - '0'};
            for (int j{size}; j > 0; --j)
            {
                if (i % 2 == 0)
                    blocks.push_back(fileID);
                else
                    blocks.push_back(free);
            }
            if (i % 2 == 0)
                ++fileID;
        }

        return blocks;
    }

    std::vector<int> defragmentSystem(const std::vector<int>& blocks)
    {
        std::vector<int> processed {blocks};

        for (size_t i{0}; i < processed.size(); ++i)
        {
            if (processed[i] == -1)
            {
                size_t lastIndex {};
                bool indexFound {false};
                for (size_t j{processed.size() - 1}; j > i; --j)
                {
                    if (processed[j] != -1)
                    {
                        lastIndex = j;
                        indexFound = true;
                        break;
                    }
                }
                if (indexFound)
                {
                    processed[i] = processed[lastIndex];
                    processed[lastIndex] = -1;
                }
            }
        }

        return processed;
    }

    long long getChecksum(std::vector<int> filesystem)
    {
        long long sum {0};
        int position {-1};

        for (int i : filesystem)
        {
            ++position;
            if (i == -1)
                break;
            sum += position * i;
        }

        return sum;
    }

    long long parseAndGetChecksum(std::string_view input)
    {
        std::vector<int> blocks {getIndividualBlocks(input)};
        std::vector<int> filesystem {defragmentSystem(blocks)};

        return getChecksum(filesystem);
    }
}

#endif // AOC9A_H
