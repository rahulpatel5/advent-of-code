#ifndef AOC11B_H
#define AOC11B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <ranges>
#include <cassert>
#include "Network.h"

/*
FOR each path (following output):
    TRACK if output is one of visit spots
    IF reach out and visited both visit spots:
        ADD to count

RETURN count

taking too long so try different approach
set up puzzle as a graph

SET UP graph of devices
FOR each cycle with start, end and visited location:
    ADD to count

RETURN count

takes too long to go start to finish
try splitting out and finding different paths for:
    svr -> fft
    svr -> dac
    fft -> dac
    dac -> fft
    fft -> out
    dac -> out
and combine these to get the final answer

use graphs to find these connections
*/

namespace aoc11b
{
    using PathInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using DeviceStr = std::string;
    using Device = long long;
    using DeviceStrToInt = std::map<DeviceStr, Device>;
    using DeviceIntToStr = std::map<Device, DeviceStr>;
    using Devices = std::vector<Device>;
    using DeviceMap = std::map<Device, Devices>;

    using UniqueKeys = std::set<Device>;

    template <std::size_t L>
    void readDevices(const Lines<L>& lines, DeviceMap& map, DeviceStrToInt& strToInt, DeviceIntToStr& intToStr)
    {
        long long index {0};
        for (std::string_view line : lines)
        {
            if (line.find(':') == std::string_view::npos)
                continue;
            
            std::string row { std::string(line) };
            size_t colon { line.find(':') };
            DeviceStr key { line.substr(0, colon) };
            if (strToInt.find(key) == strToInt.end())
            {
                strToInt[key] = index;
                intToStr[index] = key;
                ++index;
            }

            Devices outputs {};
            size_t space { line.find(' ') };
            while (space != std::string::npos)
            {
                size_t nextSpace { line.find(' ', space + 1) };
                DeviceStr output {""};
                if (nextSpace != -1)
                    output = line.substr(space + 1, nextSpace - (space + 1));
                else
                    output = line.substr(space + 1);
                if (strToInt.find(output) == strToInt.end())
                {
                    strToInt[output] = index;
                    intToStr[index] = output;
                    ++index;
                }
                outputs.push_back(strToInt.at(output));
                space = nextSpace;
            }

            map[strToInt.at(key)] = outputs;
        }
    }

    void checkUniqueKeys(const DeviceMap& map)
    {
        UniqueKeys unique {};
        for (const Device& key : map | std::ranges::views::keys)
        {
            unique.insert(key);
        }
        assert(map.size() == unique.size() && "Assumed that there were unique keys. But there are not. Need to change approach.");
    }

    template <std::size_t L>
    PathInt countPaths(const Lines<L>& lines)
    {
        DeviceMap deviceMap {};
        DeviceStrToInt deviceStrToInt {};
        DeviceIntToStr deviceIntToStr {};
        readDevices<L>(lines, deviceMap, deviceStrToInt, deviceIntToStr);
        checkUniqueKeys(deviceMap);

        Network reverseNetwork(deviceMap.size());
        for (const auto& [dev, outputs] : deviceMap)
        {
            for (const Device& output : outputs)
            {
                reverseNetwork.addEdge(output, dev);
            }
        }

        PathInt svrTofft { reverseNetwork.findPathsBetween(deviceStrToInt.at("svr"), deviceStrToInt.at("fft"), deviceStrToInt.at("dac"), deviceStrToInt.at("out")) };

        PathInt svrTodac { reverseNetwork.findPathsBetween(deviceStrToInt.at("svr"), deviceStrToInt.at("dac"), deviceStrToInt.at("fft"), deviceStrToInt.at("out")) };

        // no point following a path if getting 0 for prior path
        PathInt fftTodac {0};
        if (svrTofft > 0)
            fftTodac = reverseNetwork.findPathsBetween(deviceStrToInt.at("fft"), deviceStrToInt.at("dac"), deviceStrToInt.at("svr"), deviceStrToInt.at("out"));

        PathInt dacTofft {0};
        if (svrTodac > 0)
            dacTofft = reverseNetwork.findPathsBetween(deviceStrToInt.at("dac"), deviceStrToInt.at("fft"), deviceStrToInt.at("svr"), deviceStrToInt.at("out"));

        PathInt dacToout {0};
        if (svrTofft > 0 && fftTodac > 0)
            dacToout = reverseNetwork.findPathsBetween(deviceStrToInt.at("dac"), deviceStrToInt.at("out"), deviceStrToInt.at("svr"), deviceStrToInt.at("fft"));

        PathInt fftToout {0};
        if (svrTodac > 0 && dacTofft > 0)
            fftToout = reverseNetwork.findPathsBetween(deviceStrToInt.at("fft"), deviceStrToInt.at("out"), deviceStrToInt.at("svr"), deviceStrToInt.at("dac"));

        return (svrTofft * fftTodac * dacToout) + (svrTodac * dacTofft * fftToout);
    }
}

#endif // AOC11B_H
