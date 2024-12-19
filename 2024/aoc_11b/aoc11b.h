#ifndef AOC11B_H
#define AOC11B_H

#include <string_view>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>
#include <set>
#include <stdexcept>

/*
try part 1 solution and go from there
even test solution almost halts on the 48th iteration due to ballooning size
idea to extend memoisation to handle repeating patterns in numbers
e.g. 2024 -> [20, 24] -> [2, 0, 2, 4] -> ...
1) question of whether there are numbers that loop. If so, can use this
2) question of which numbers loop and how to identify/collect them
for numbers under 0:
0 -> 1 -> 2024 -> 20, 24 -> 2, 0, 2, 4
1 -> 2024 -> ...
2 -> 4048 -> 40, 48 -> 4, 0, 4, 8
3 -> 6072 -> 60, 64 -> 6, 0, 7, 2
4 -> 8096 -> 80, 96 -> 8, 0, 9, 6
5 -> 10120 -> 20482880 -> ... -> 2, 0, 4, 8, 2, 8, 8, 0
6 -> 12144 -> 24579456 -> ... -> 2, 4, 5, 7, 9, 4, 5, 6
7 -> 14168 -> 28676032 -> ... -> 2, 8, 6, 7, 6, 0, 3, 2
8 -> 16192 -> 32772608 -> ... -> 3, 2, 7, 7, 2, 6, 0, 8
9 -> 18216 -> 36869184 -> ... -> 3, 6, 8, 6, 9, 1, 8, 4

initial plan is to change memoisation to collect these patterns
i.e. output number of stones being added, rather than stones themselves
it may be the size of the vectors being collected that's causing issues

added second layer of memoisation to make problem possible
could add a third layer of memoisation to improve speed?
i.e. most time taken by first few loops, which traverse all/most iterations
could collect those in another layer of memoisation to reduce time?
though later loops are faster due to the front-loading of computation
so might not make as much difference as somehow improving the existing code
e.g. using any pattern in the second layer of memoisation?

could swap between first and second layer of memoisation
first layer collects numbers and may be more efficient at early stages
second layer is based on number of iterations so is faster at later stages
could try different values for swapping between them?

can get multiples of stones at a given point, e.g. 1 -> ... -> 2, 0, 2, 4
instead of handling each repeat, could multiply the size of one and drop the other
would need to keep the multiplier going to the end

testing suggests the file runs fastest when
ITERATION_SWITCH ~= BLINKS / 2
e.g. fastest for
BLINKS      ITERATION_SWITCH
25          10
50          25
60          32
so could try ITERATION_SWITCH as c39 or 40, for the puzzle

getting int (long long) overflow
need to consider if it's expected that would be the case (i.e. if the solution is wrong)
or, if this could be the case, either a larger int container (is that possible?) or tracking where overflow may occur?
    TO TRY: use unsigned long long?
            create a new int container? https://stackoverflow.com/a/5382282
*/

namespace constants
{
    constexpr int BLINKS {75};
    constexpr int MULTIPLIER {2024};
    constexpr double MULTIPLIER_DOUBLE {2024.0};
    constexpr double MANTISSA {10000.0 / MULTIPLIER};
    constexpr int ITERATION_SWITCH {39};
}

namespace aoc11b
{
    // clean up long types being used
    using Memo = std::map<long long, std::vector<long long>>;
    using MemoDict = std::map<std::pair<long long, int>, long long>;
    using SizeList = std::vector<long long>;
    using SizeLists = std::vector<SizeList>;
    using IndexList = std::vector<size_t>;
    // set up a size count type alias to simplify changing this
    using SizeCount = unsigned long long;

    std::vector<long long> getInitialStones(std::string_view input)
    {
        std::vector<long long> stones {};
        size_t start {0};
        size_t end {0};
        while (end != std::string_view::npos)
        {
            end = input.find(' ', start);
            long long num {std::stoll(std::string(input.substr(start, end)))};
            stones.push_back(num);
            start = end + 1;
        }
        return stones;
    }

    bool isEvenNumberOfDigits(long long num)
    {
        std::string digit {std::to_string(num)};
        return digit.size() % 2 == 0;
    }

    std::vector<long long> splitAndAddStones(long long stone)
    {
        std::string originalStone {std::to_string(stone)};
        size_t splitPoint {originalStone.size() / 2};
        std::string leftStone {originalStone.substr(0, splitPoint)};
        std::string rightStone {originalStone.substr(splitPoint)};
        return {std::stoll(leftStone), std::stoll(rightStone)};
    }

    bool willStayOddNumberOfDigits(long long num, int iteration)
    {
        int exponent {(int)std::floor(std::log10(num))};
        double significand {(double)num * std::pow(10, -exponent)};
        for (int i{iteration}; i < constants::BLINKS; ++i)
        {
            significand *= constants::MULTIPLIER_DOUBLE;
            int exp {(int)std::floor(std::log10(significand))};
            significand *= std::pow(10, -exp);
            exponent += exp;
            // an even numbered exponent means an odd number of digits
            // account for this when identifying odd number of digits
            if ((exponent + 1) % 2 == 0)
                return false;
        }
        return true;
    }

    bool isPreviousSizeCount(long long stone, int iteration, const MemoDict& memoSizeDict)
    {
        return memoSizeDict.count({stone, iteration}) > 0;
    }

    // this function has a side-effect of updating memoSizeDict
    SizeCount addSizeCountForIterations(long long stone, int iteration, const Memo& memoSize, const Memo& memoNext, MemoDict& memoSizeDict)
    {
        int startIteration {iteration};
        // start with 1 for the current stone
        SizeCount sizeCount {1};
        SizeLists stoneSizes {};
        // start by adding starting stone
        stoneSizes.push_back(memoSize.at(stone));
        // track which stone started each list
        SizeList stoneTracker {};
        stoneTracker.push_back(stone);
        // use multiplier to reduce repetition
        SizeList sizeMultiplier {};
        sizeMultiplier.push_back(1);
        while (iteration < constants::BLINKS)
        {
            SizeList stonesToAdd {};
            IndexList stonesToRemove {};
            SizeList multipliersToAdd {};
            for (size_t i{0}; i < stoneSizes.size(); ++i)
            {
                // add extra stone(s) using difference with old iteration
                // multiply by multiplier to account for dropped paths
                sizeCount += (stoneSizes[i].at(1) - stoneSizes[i].at(0)) * sizeMultiplier[i];
                // remove the front of the vector to move along it
                stoneSizes[i].erase(stoneSizes[i].begin());
                // if have a vector of 1, then have reached the end
                if (stoneSizes[i].size() == 1)
                {
                    // reached the end of the previous stone list
                    // set next set of stones to add
                    // need to do this outside the stoneSizes for loop
                    // collect stones to add for now
                    stonesToAdd.push_back(stoneTracker[i]);
                    multipliersToAdd.push_back(sizeMultiplier[i]);
                    // clean up by deleting the finished vector
                    // as above, need to collect this and do after for loop
                    stonesToRemove.push_back(i);
                }
            }
            std::sort(stonesToRemove.rbegin(), stonesToRemove.rend());
            for (size_t index : stonesToRemove)
            {
                stoneSizes.erase(stoneSizes.begin() + index);
                stoneTracker.erase(stoneTracker.begin() + index);
                sizeMultiplier.erase(sizeMultiplier.begin() + index);
            }
            for (size_t addIndex{0}; addIndex < stonesToAdd.size(); ++addIndex)
            {
                // need to handle multipliers
                std::set<long long> duplicateMultiplers {};
                for (long long nextStone : memoNext.at(stonesToAdd[addIndex]))
                {
                    // otherwise skip if already handled a duplicate stone
                    if (duplicateMultiplers.count(nextStone) > 0)
                        continue;
                    stoneSizes.push_back(memoSize.at(nextStone));
                    stoneTracker.push_back(nextStone);
                    long long additionalMultiplier {std::count(memoNext.at(stonesToAdd[addIndex]).begin(), memoNext.at(stonesToAdd[addIndex]).end(), nextStone)};
                    if (additionalMultiplier > 1)
                        duplicateMultiplers.insert(nextStone);
                    sizeMultiplier.push_back(multipliersToAdd[addIndex] * additionalMultiplier);
                }
            }
            // collect size data as we go, to save time/effort later
            memoSizeDict[{stone, (constants::BLINKS - 1) - (iteration - startIteration)}] = sizeCount;
            ++iteration;
        }
        return sizeCount;
    }

    std::vector<long long> applyBlinkRules(std::vector<long long>& stones, Memo& memo, int iteration, const Memo& memoSize, const Memo& memoNext, SizeCount& sizeCount, MemoDict& memoSizeDict)
    {
        std::vector<long long> blinkStones {};
        for (size_t i{0}; i < stones.size(); ++i)
        {
            // add handling of change in number of stones
            // to make this efficient enough to get an answer
            // this identifies if stone is 0-9 (or 16,192)
            // use iterations left to get total stones added
            // also collect these iterations to re-use for later
            if (isPreviousSizeCount(stones[i], iteration, memoSizeDict))
                sizeCount += memoSizeDict.at({stones[i], iteration});
            else if (memoSize.count(stones[i]) > 0 && iteration > constants::ITERATION_SWITCH)
            {
                // following function has a side-effect of updating memoSizeDict
                SizeCount extraSize {addSizeCountForIterations(stones[i], iteration, memoSize, memoNext, memoSizeDict)};
                sizeCount += extraSize;
                memoSizeDict[{stones[i], iteration}] = extraSize;
            }

            // rest is as for the part 1 solution
            else if (memo.count(stones[i]) > 0)
            {
                for (long long value : memo[stones[i]])
                    blinkStones.push_back(value);
            }

            else if (isEvenNumberOfDigits(stones[i]))
            {
                std::vector<long long> split {splitAndAddStones(stones[i])};
                memo[stones[i]] = split;
                for (long long value : split)
                    blinkStones.push_back(value);
            }
            else
            {
                if (willStayOddNumberOfDigits(stones[i], iteration))
                {
                    // this may update the map (memo) value
                    // this is intended, as a number may not reach a point
                    // where splitting is possible after enough iterations
                    memo[stones[i]] = {stones[i]};
                    blinkStones.push_back(stones[i]);
                }
                else
                {
                    memo[stones[i]] = {stones[i] * constants::MULTIPLIER};
                    blinkStones.push_back(stones[i] * constants::MULTIPLIER);
                }
            }
        }
        return blinkStones;
    }

    bool areAllNumbersSingleDigit(const std::vector<long long>& arr)
    {
        for (long long num : arr)
        {
            if (num > 9)
                return false;
            else if (num < 0)
                throw std::invalid_argument("Not expecting number less than 0.\n");
        }
        return true;
    }

    void startUpMemo(Memo& memo)
    {
        // we want to set up memo for all values in the 0-9 loop
        for (long long n{0}; n < 10; ++n)
        {
            std::vector<long long> memoCollection {n};
            do
            {
                std::vector<long long> newCollection {};
                for (long long num : memoCollection)
                {
                    // we can skip where we already have number memo'd
                    if (memo.count(num) > 0)
                        continue;
                    // handle if even number of digits
                    else if (isEvenNumberOfDigits(num))
                    {
                        std::vector<long long> split {splitAndAddStones(num)};
                        memo[num] = split;
                        for (long long value : split)
                            newCollection.push_back(value);
                    }
                    // otherwise multiply by multiplier
                    else
                    {
                        memo[num] = {num * constants::MULTIPLIER};
                        newCollection.push_back(num * constants::MULTIPLIER);
                    }
                }
                memoCollection = newCollection;
            } while (!areAllNumbersSingleDigit(memoCollection));
        }
    }

    Memo setUpMemoSize(const Memo& memo)
    {
        Memo memoSize {};
        for (long long i{0}; i < 10; ++i)
        {
            std::vector<long long> collection {};
            collection.push_back(i);
            std::vector<long long> size {static_cast<long long>(collection.size())};

            do
            {
                std::vector<long long> loopCollection {};
                for (long long num : collection)
                {
                    std::vector<long long> values {memo.at(num)};
                    for (long long value : values)
                        loopCollection.push_back(value);
                }
                collection = loopCollection;
                size.push_back(collection.size());
                // can't think of a better way to handle 8
                if (i == 8 && collection.size() == 7)
                    break;
            } while (!areAllNumbersSingleDigit(collection));
            memoSize[i] = size;
        }

        // need to handle 8 since it doesn't end in single digit numbers
        // it has 16,192 as part of its ending list
        std::vector<long long> collectExtra {};
        collectExtra.push_back(16192);
        std::vector<long long> sizeExtra {static_cast<long long>(collectExtra.size())};
        // like 8, 16,192 doesn't get single digits only
        // need to end at a similar point to 8
        while (collectExtra.size() < 7)
        {
            std::vector<long long> loopExtra {};
                for (long long num : collectExtra)
                {
                    std::vector<long long> valuesExtra {memo.at(num)};
                    for (long long value : valuesExtra)
                        loopExtra.push_back(value);
                }
                collectExtra = loopExtra;
                sizeExtra.push_back(collectExtra.size());
        }
        memoSize[16192] = sizeExtra;

        return memoSize;
    }

    Memo getMemoNextNumbers(const Memo& memo, const Memo& memoSize)
    {
        Memo memoNext {};
        for (const auto& [key, values] : memoSize)
        {
            std::vector<long long> collection {key};
            for (int i{1}; i < values.size(); ++i)
            {
                std::vector<long long> tempColl {};
                for (long long value : collection)
                {
                    for (long long val : memo.at(value))
                        tempColl.push_back(val);
                }
                collection = tempColl;
            }
            memoNext[key] = collection;
        }
        return memoNext;
    }

    SizeCount blinkForNewStones(const std::vector<long long>& stones)
    {
        std::vector<long long> blinkStones {stones};

        // using memoisation to store previous values;
        Memo memo {};
        memo[0] = {1};
        // set up for single digit numbers in first instance
        // as well as for numbers in the 0-9 loop (see comments at the top of file)
        startUpMemo(memo);

        // add second memoisation using sizes, instead of numbers directly
        Memo memoSize {setUpMemoSize(memo)};
        // add associated memoisation for numbers at end of sizes array
        Memo memoNext {getMemoNextNumbers(memo, memoSize)};
        // add third layer memoisation for above sizes
        MemoDict memoSizeDict {};

        // keep count of numbers of stones by size, to add back later
        SizeCount sizeCount {0};
        for (int i{0}; i < constants::BLINKS; ++i)
            blinkStones = applyBlinkRules(blinkStones, memo, i, memoSize, memoNext, sizeCount, memoSizeDict);
        return sizeCount + blinkStones.size();
    }

    SizeCount parseAndCountStones(std::string_view input)
    {
        std::vector<long long> startStones {getInitialStones(input)};
        return blinkForNewStones(startStones);
    }
}

#endif // AOC11B_H
