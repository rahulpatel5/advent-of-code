#ifndef AOC9A_H
#define AOC9A_H

#include <string_view>
#include <vector>
#include <string>

/*
process string into vector (because integers can go above single digits)
rearrange vector to move later numbers to fill earlier 'empty' ones
keep track of left position, to not waste time like an idiot
calculate checksum
*/

namespace aoc9a
{
    std::vector<int> getIndividualBlocks(const std::string_view& input, size_t& finalBlockPosition)
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
            {
                finalBlockPosition += size;
                ++fileID;
            }
        }

        return blocks;
    }

    std::vector<int> defragmentSystem(const std::vector<int>& blocks, size_t finalBlockPosition)
    {
        std::vector<int> processed {blocks};
        long long currentPosition {0};

        for (long long i{static_cast<long long>(processed.size() - 1)}; i >= 0; --i)
        {
            if (i == finalBlockPosition)
                break;

            if (processed[i] == -1)
                continue;
            
            for (; currentPosition < static_cast<long long>(processed.size()); ++currentPosition)
            {
                if (processed.at(currentPosition) == -1)
                {
                    processed[currentPosition] = processed[i];
                    processed[i] = -1;
                    ++currentPosition;
                    break;
                }
            }
        }

        return processed;
    }

    long long getChecksum(const std::vector<int>& filesystem)
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

    long long parseAndGetChecksum(const std::string_view& input)
    {
        size_t finalBlockPosition {0};
        std::vector<int> blocks {getIndividualBlocks(input, finalBlockPosition)};
        std::vector<int> filesystem {defragmentSystem(blocks, finalBlockPosition)};

        return getChecksum(filesystem);
    }
}

#endif // AOC9A_H
