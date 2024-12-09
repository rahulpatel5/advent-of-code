#ifndef AOC9B_H
#define AOC9B_H

#include <string_view>
#include <vector>
#include <string>
#include <array>
#include <set>

/*
process string into vector as in aoc9a
iterate vector in reverse and collect files (ints) and file sizes
iterate vector forward and find empty locations
place files in spaces, with latest tried first. Skip any files that don't have space to fit in earlier
calculate checksum as in aoc9a
*/

namespace aoc9b
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

    std::vector<std::array<int, 3>> getFileInformation(const std::vector<int>& blocks)
    {
        std::vector<std::array<int, 3>> fileInfo {};
        int size {0};
        int currentFile {};

        // collect files in reverse order, including file number, size and location
        // using int here 1) to avoid signed int overflowing, and 2) as collecting a vector of ints
        for (int i{static_cast<int>(blocks.size() - 1)}; i >= 0; --i)
        {
            if (i == blocks.size() - 1)
            {
                currentFile = blocks[i];
                size = 1;
                continue;
            }

            if (blocks[i] == currentFile)
                ++size;
            else
            {
                fileInfo.push_back({currentFile, size, i + 1});
                size = 1;
                currentFile = blocks[i];
            }
            // need to handle final loop
            if (i == 0)
                fileInfo.push_back({currentFile, size, i});
        }
        return fileInfo;
    }

    size_t getLargeEnoughFreeSpace(const std::vector<int>& blocks, int fileSize, int fileIndex)
    {
        int freeSize {0};
        int freeSpace {-1};
        size_t freeIndex {static_cast<size_t>(fileIndex + 1)}; // default value is sentinel value (index of free space must be before file)

        for (size_t i{0}; i < fileIndex; ++i)
        {
            if (blocks[i] != freeSpace)
            {
                freeSize = 0;
                continue;
            }
            ++freeSize;
            if (freeSize == fileSize)
            {
                freeIndex = i - (freeSize - 1);
                return freeIndex;
            }
        }
        // this should be the default sentinel value
        return freeIndex;
    }

    void moveFile (std::vector<int>& blocks, int fileIndex, size_t moveIndex, int fileSize)
    {
        for (int i{0}; i < fileSize; ++i)
        {
            blocks[moveIndex + i] = blocks[fileIndex + i];
            blocks[fileIndex + i] = -1;
        }
    }

    std::vector<int> iterateAndMoveFiles(const std::vector<int>& blocks)
    {
        // elements: 1) file number, 2) file size, 3) file index (earliest)
        std::vector<std::array<int, 3>> fileInfo {getFileInformation(blocks)};

        std::vector<int> processed {blocks};

        for (std::array<int, 3> file : fileInfo)
        {
            if (file[0] == -1)
                continue;
            size_t moveIndex {getLargeEnoughFreeSpace(processed, file[1], file[2])};
            if (moveIndex < file[2])
                moveFile(processed, file[2], moveIndex, file[1]);
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
                continue;
            sum += position * i;
        }

        return sum;
    }

    long long parseAndGetChecksum(std::string_view input)
    {
        std::vector<int> blocks {getIndividualBlocks(input)};
        std::vector<int> filesystem {iterateAndMoveFiles(blocks)};

        return getChecksum(filesystem);
    }
}

#endif // AOC9B_H
