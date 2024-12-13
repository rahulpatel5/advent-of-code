#ifndef AOC12A_H
#define AOC12A_H

#include <string_view>
#include <array>
#include <vector>
#include <set>
#include <cassert>

/*
iterate map
take each type of plant (letter)
find region for that starting plant, i.e. that are connected (use std::set)
as have that region, continue through map and find other regions of the same plant (letter) and collect those regions
with that collection (vector set?), count respective area and perimeter
continue through rest of map, collecting other plants (ignore previous type)
get price of fencing
*/

namespace aoc12a
{
    using PlantRegions = std::vector<std::set<std::pair<size_t, size_t>>>;
    using PlantRegion = std::set<std::pair<size_t, size_t>>;

    bool isAlreadyInRegion(size_t row, size_t col, const PlantRegion& region)
    {
        for (std::pair<size_t, size_t> plot : region)
        {
            if (plot.second == row && plot.first == col)
                return true;
        }
        return false;
    }

    bool isNotAlreadyInRegions(size_t row, size_t col, const PlantRegions& regions)
    {
        for (const PlantRegion& region : regions)
        {
            if (isAlreadyInRegion(row, col, region))
                return false;
        }
        return true;
    }

    template <std::size_t N>
    void recursiveGetRegion(const std::set<std::pair<size_t, size_t>>& currentPlots, char plant, const std::array<std::string_view, N>& lines, PlantRegion& region)
    {
        std::set<std::pair<size_t, size_t>> newPlots {};
        for (std::pair<size_t, size_t> plot : currentPlots)
        {
            size_t row {plot.second};
            size_t col {plot.first};
            // check north
            if ((row > 0) && (lines[row - 1][col] == plant) && !isAlreadyInRegion(row - 1, col, region))
            {
                std::pair<size_t, size_t> newPlot {col, row - 1};
                region.insert(newPlot);
                newPlots.insert(newPlot);
            }
            // check east
            if ((col < lines[0].size() - 1) && (lines[row][col + 1] == plant) && !isAlreadyInRegion(row, col + 1, region))
            {
                std::pair<size_t, size_t> newPlot {col + 1, row};
                region.insert(newPlot);
                newPlots.insert(newPlot);
            }
            // check south
            if ((row < lines.size() - 1) && (lines[row + 1][col] == plant) && !isAlreadyInRegion(row + 1, col, region))
            {
                std::pair<size_t, size_t> newPlot {col, row + 1};
                region.insert(newPlot);
                newPlots.insert(newPlot);
            }
            // check west
            if ((col > 0) && (lines[row][col - 1] == plant) && !isAlreadyInRegion(row, col - 1, region))
            {
                std::pair<size_t, size_t> newPlot {col - 1, row};
                region.insert(newPlot);
                newPlots.insert(newPlot);
            }
        }
        
        if (newPlots.size() == 0)
            return;
        else
            return recursiveGetRegion(newPlots, plant, lines, region);
    }

    template <std::size_t N>
    PlantRegions getPlantRegions(char plant, size_t row, size_t col, const std::array<std::string_view, N>& lines)
    {
        PlantRegions regions {};

        for (size_t i{row}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines[i].size(); ++j)
            {
                if (i == row && j < col)
                    continue;
                
                if (lines[i][j] == plant && isNotAlreadyInRegions(i, j, regions))
                {
                    std::set<std::pair<size_t, size_t>> start {{std::make_pair(j, i)}};
                    PlantRegion region {{start}};
                    recursiveGetRegion(start, plant, lines, region);
                    regions.push_back(region);
                }
            }
        }

        return regions;
    }

    bool isNotProcessedPlant(char plant, const std::set<char>& processedPlants)
    {
        for (char processedPlant : processedPlants)
        {
            if (processedPlant == plant)
                return false;
        }
        return true;
    }

    std::vector<int> calculateAreas(const PlantRegions& regions)
    {
        std::vector<int> areas {};
        for (const PlantRegion& region : regions)
            areas.push_back(region.size());
        return areas;
    }

    template <std::size_t N>
    int countPerimeter(const PlantRegion& region, const std::array<std::string_view, N>& lines, char plant)
    {
        int perimeter {0};
        for (std::pair<size_t, size_t> plot : region)
        {
            size_t row {plot.second};
            size_t col {plot.first};
            // check north
            if (row == 0 || lines[row - 1][col] != plant)
                ++perimeter;
            // check east
            if (col == lines[0].size() - 1 || lines[row][col + 1] != plant)
                ++perimeter;
            if (row == lines.size() - 1 || lines[row + 1][col] != plant)
                ++perimeter;
            if (col == 0 || lines[row][col - 1] != plant)
                ++perimeter;
        }
        return perimeter;
    }

    template <std::size_t N>
    std::vector<int> calculatePerimeters(const PlantRegions& regions, const std::array<std::string_view, N>& lines, char plant)
    {
        std::vector<int> perimeters {};
        for (const PlantRegion& region : regions)
        {
            int perimeter {countPerimeter(region, lines, plant)};
            perimeters.push_back(perimeter);
        }
        return perimeters;
    }

    int calculatePlantPrice(const std::vector<int>& areas, const std::vector<int>& perimeters)
    {
        int price {0};
        assert((areas.size() == perimeters.size()) && "Plant does not have the same number of regions for areas and perimeters.");
        for (size_t i{0}; i < areas.size(); ++i)
            price += areas[i] * perimeters[i];
        return price;
    }

    template <std::size_t N>
    std::vector<int> getPricePerPlant(const std::array<std::string_view, N>& lines)
    {
        std::vector<int> prices {};
        std::set<char> processedPlants {};

        for (size_t i{0}; i < lines.size(); ++i)
        {
            for (size_t j{0}; j < lines[i].size(); ++j)
            {
                char plant {lines[i][j]};
                if (isNotProcessedPlant(plant, processedPlants))
                {
                    PlantRegions regions {getPlantRegions(plant, i, j, lines)};
                    std::vector<int> areas {calculateAreas(regions)};
                    std::vector<int> perimeters {calculatePerimeters(regions, lines, plant)};
                    int price {calculatePlantPrice(areas, perimeters)};
                    prices.push_back(price);
                    processedPlants.insert(plant);
                }
            }
        }

        return prices;
    }

    int getTotalPrice(const std::vector<int>& prices)
    {
        int total {0};
        for (int price : prices)
            total += price;
        return total;
    }

    template <std::size_t N>
    int parseAndGetFencingPrice(const std::array<std::string_view, N>& lines)
    {
        std::vector<int> prices {getPricePerPlant(lines)};
        return getTotalPrice(prices);
    }
}

#endif // AOC12A_H
