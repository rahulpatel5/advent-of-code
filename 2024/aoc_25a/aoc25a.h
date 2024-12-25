#ifndef AOC25A_H
#define AOC25A_H

#include <string_view>
#include <array>
#include <vector>

/*
sort locks and keys
get heights of each column (in vector)
for each lock, try each key
i.e. see if sum of lock and key is the total height (not more or less)
return number of pairs that fit together
*/

namespace aoc25a
{
    using KeyInt = long long;
    using Row = std::string_view;
    using Schematic = std::vector<Row>;
    using Schematics = std::vector<Schematic>;
    using Height = int;
    using Lock = std::vector<Height>;
    using Locks = std::vector<Lock>;
    using Key = std::vector<Height>;
    using Keys = std::vector<Key>;

    template <std::size_t N>
    void getLocksAndKeys(Schematics& locks, Schematics& keys, const std::array<std::string_view, N>& lines)
    {
        Schematic collection {};
        bool isLock {false};
        bool isKey {false};
        for (std::string_view line : lines)
        {
            // if we have an empty line, add the lock/key to the collection
            // and reset everything else
            if (line == "")
            {
                if (isLock)
                    locks.push_back(collection);
                else if (isKey)
                    keys.push_back(collection);
                collection = {};
                isLock = false;
                isKey = false;
                continue;
            }
            // key or lock not set at start or after a blank line
            else if (!isKey && !isLock)
            {
                if (line[0] == '#')
                    isLock = true;
                else if (line[0] == '.')
                    isKey = true;
            }
            collection.push_back(line);
        }
        if (isLock)
            locks.push_back(collection);
        else if (isKey)
            keys.push_back(collection);
    }

    // function has return output of either Locks or Keys
    Locks getHeights(const Schematics& schematics, char pin)
    {
        Locks allHeights {};
        for (const Schematic& schematic : schematics)
        {
            Lock heights {};
            for (std::string_view line : schematic)
            {
                for (size_t x{0}; x < line.size(); ++x)
                {
                    if (heights.size() == 0 || heights.size() - 1 < x)
                    {
                        if (line.at(x) == pin)
                            heights.push_back(1);
                        else
                            heights.push_back(0);
                    }
                    else
                    {
                        if (line.at(x) == pin)
                            heights[x] += 1;
                    }
                }
            }
            allHeights.push_back(heights);
        }
        return allHeights;
    }

    bool isFit(const Lock& lock, const Key& key, size_t size)
    {
        for (size_t col{0}; col < lock.size(); ++col)
        {
            if (lock.at(col) + key.at(col) > size)
                return false;
        }
        return true;
    }

    KeyInt countFittingPairs(const Locks& locks, const Keys& keys, size_t columnSize)
    {
        KeyInt count {0};
        for (const Lock& lock : locks)
        {
            for (const Key& key : keys)
            {
                if (isFit(lock, key, columnSize))
                    ++count;
            }
        }
        return count;
    }

    template <std::size_t N>
    KeyInt parseAndGetLockKeyPairs(const std::array<std::string_view, N>& lines)
    {
        Schematics locks {};
        Schematics keys {};
        getLocksAndKeys<N>(locks, keys, lines);
        Locks heightLocks {getHeights(locks, '#')};
        Keys heightKeys {getHeights(keys, '#')};
        size_t columnSize {locks.at(0).size()};
        return countFittingPairs(heightLocks, heightKeys, columnSize);
    }
}

#endif // AOC25A_H
