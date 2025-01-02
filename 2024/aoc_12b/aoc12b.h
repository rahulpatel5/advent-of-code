#ifndef AOC12B_H
#define AOC12B_H

#include <string_view>
#include <array>
#include <vector>
#include <set>
#include <cassert>
#include <stdexcept>

/*
re-use solution for part a to get regions as usual
change perimeter function to a function calculating number of sides
could check x and y coordinates, e.g. where y = 0 and x is [0,5] and there are none in the region on y = 1
could follow perimeter around the region, if that's possible?
get price of fencing (area * sides)
ISSUE:
    need to handle inner perimeter, as referenced in puzzle text
    perhaps identify such regions specifically and handle them?
    could invert the logic and have the perimeter be between the plant type and not plant type?
*/

namespace direction
{
    constexpr std::pair<int, int> north { 0, -1};
    constexpr std::pair<int, int> east  { 1,  0};
    constexpr std::pair<int, int> south { 0,  1};
    constexpr std::pair<int, int> west  {-1,  0};

    constexpr std::array<std::pair<int, int>, 4> allDirections {north, east, south, west};
}

// custom comparator for set to order by row first
struct CompareByRow
{
    bool operator()(const std::pair<int, int>& left, const std::pair<int, int>& right) const
    {
        if (left.second != right.second)
            return left.second < right.second;
        else
            return left.first < right.first;
    }
};

namespace aoc12b
{
    using PlantRegion = std::set<std::pair<size_t, size_t>>;
    using PlantRegions = std::vector<PlantRegion>;
    using PlantRegionByRow = std::set<std::pair<size_t, size_t>, CompareByRow>;

    bool isAlreadyInRegion(size_t row, size_t col, const PlantRegion& region)
    {
        return region.find({col, row}) != region.end();
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
    bool isMovingIntoWall(std::pair<size_t, size_t> point, std::pair<int, int> move, const std::array<std::string_view, N>& lines)
    {
        return ((move == direction::north && point.second == 0) || (move == direction::east && point.first == lines[0].size() - 1) || (move == direction::south && point.second == lines.size() - 1) || (move == direction::west && point.first == 0));
    }

    std::pair<int, int> turnAntiClockwise(std::pair<int, int> move)
    {
        if (move == direction::north)
            return direction::west;
        else if (move == direction::east)
            return direction::north;
        else if (move == direction::south)
            return direction::east;
        else if (move == direction::west)
            return direction::south;
        else
            throw std::invalid_argument("Did not have a valid starting direction.");
    }

    std::pair<int, int> turnClockwise(std::pair<int, int> move)
    {
        if (move == direction::north)
            return direction::east;
        else if (move == direction::east)
            return direction::south;
        else if (move == direction::south)
            return direction::west;
        else if (move == direction::west)
            return direction::north;
        else
            throw std::invalid_argument("Did not have a valid starting direction.");
    }

    std::pair<size_t, size_t> nextPosition(std::pair<size_t, size_t> point, std::pair<int, int> move)
    {
        return {point.first + move.first, point.second + move.second};
    }

    template <std::size_t N>
    bool isNextPositionValid(std::pair<size_t, size_t> point, std::pair<int, int> move, std::pair<size_t, size_t> newPoint, char plant, const std::array<std::string_view, N>& lines)
    {
        // check if next point would be the same plant type
        if (lines[newPoint.second][newPoint.first] != plant)
            return false;

        // check if travelling border of map
        if ((move == direction::north && point.first == 0) || (move == direction::east && point.second == 0) || (move == direction::south && point.first == lines[0].size() - 1) || (move == direction::west && point.second == lines.size() - 1))
            return true;
        
        std::pair<int, int> lateralMove {turnAntiClockwise(move)};
        std::pair<size_t, size_t> checkingPoint {newPoint.first + lateralMove.first, newPoint.second + lateralMove.second};
        return lines[checkingPoint.second][checkingPoint.first] != plant;
    }

    template <std::size_t N>
    bool isDiagonal(std::pair<size_t, size_t> point, std::pair<int, int> move, std::pair<size_t, size_t> forwardPoint, std::pair<size_t, size_t> diagonalPoint, char plant, const std::array<std::string_view, N>& lines)
    {
        // this is used alongside isMovingIntoWall, so that's checked already
        return ((lines[point.second][point.first] == plant) && (lines[forwardPoint.second][forwardPoint.first] != plant) && (lines[diagonalPoint.second][diagonalPoint.first] == plant));
    }

    template <std::size_t N>
    int travelPerimeter(std::pair<size_t, size_t> point, std::pair<int, int> move, std::pair<size_t, size_t> startPoint, std::pair<int, int> finalMovement, char plant, int sides, const std::array<std::string_view, N>& lines)
    {
        if (point.first == startPoint.first && point.second == startPoint.second && move == finalMovement)
            return sides;
        
        // check if can continue in the same direction
        if (!isMovingIntoWall(point, move, lines) && isNextPositionValid(point, move, nextPosition(point, move), plant, lines))
            return travelPerimeter(nextPosition(point, move), move, startPoint, finalMovement, plant, sides, lines);

        // check if can go anti-clockwise instead
        // make sure intermediary position is valid
        else if (!isMovingIntoWall(point, move, lines) && isNextPositionValid(nextPosition(point, move), turnAntiClockwise(move), nextPosition(nextPosition(point, move), turnAntiClockwise(move)), plant, lines) && !isDiagonal(point, move, nextPosition(point, move), nextPosition(nextPosition(point, move), turnAntiClockwise(move)), plant, lines))
            return travelPerimeter(nextPosition(point, move), turnAntiClockwise(move), startPoint, finalMovement, plant, ++sides, lines);
        
        // check if can go clockwise instead
        else if (isNextPositionValid(point, turnClockwise(move), point, plant, lines))
            return travelPerimeter(point, turnClockwise(move), startPoint, finalMovement, plant, ++sides, lines);
        
        // shouldn't reach here
        else
            throw std::invalid_argument("Reached an impossible position.");
    }

    template <std::size_t N>
    int countSides(const PlantRegion& region, const std::array<std::string_view, N>& lines, char plant)
    {
        // if only 1 or 2 plots in the region, it must have 4 sides
        if (region.size() == 1 || region.size() == 2)
            return 4;
        
        int sides {0};
        std::pair<size_t, size_t> startPoint {*region.begin()};
        std::pair<size_t, size_t> point {startPoint};
        // assuming that all starting points are in top-left
        // i.e. the starting side is above
        // this assumption may be a potential point of failure
        std::pair<int, int> movement {direction::east};
        std::pair<int, int> finalMovement {direction::north};
        return travelPerimeter(point, movement, startPoint, finalMovement, plant, 1, lines);
    }

    // adapted version of above function
    template <std::size_t N>
    bool isNextPositionValidSwitched(std::pair<size_t, size_t> point, std::pair<int, int> move, std::pair<size_t, size_t> newPoint, char plant, const std::array<std::string_view, N>& lines)
    {
        // check if next point would be the plant type
        // remember we've switched the logic in this version of the function
        if (lines[newPoint.second][newPoint.first] == plant)
            return false;

        // check if travelling border of map
        if ((move == direction::north && point.first == 0) || (move == direction::east && point.second == 0) || (move == direction::south && point.first == lines[0].size() - 1) || (move == direction::west && point.second == lines.size() - 1))
            return true;
        
        std::pair<int, int> lateralMove {turnAntiClockwise(move)};
        std::pair<size_t, size_t> checkingPoint {newPoint.first + lateralMove.first, newPoint.second + lateralMove.second};
        return lines[checkingPoint.second][checkingPoint.first] == plant;
    }

    // template <std::size_t N>
    // bool isSurrounded(std::pair<size_t, size_t> point, char plant, const std::array<std::string_view, N>& lines)
    // {
    //     for (std::pair<int, int> move : direction::allDirections)
    //     {
    //         if (!isMovingIntoWall(point, move, lines) && lines[point.second + move.second][point.first + move.first] != plant)
    //             return false;
    //     }
    //     return true;
    // }

    // mostly similar to the travelPerimeter function above
    template <std::size_t N>
    int travelInnerPerimeter(std::pair<size_t, size_t> point, std::pair<int, int> move, std::pair<size_t, size_t> startPoint, std::pair<int, int> finalMovement, char plant, int sides, const std::array<std::string_view, N>& lines, PlantRegion& unique, const PlantRegion& region)
    {
        // things have gone wrong if we've circled around our region
        auto it {region.begin()};
        if (point.first < it->first && point.second < it->second)
            return 0;

        if (point.first == startPoint.first && point.second == startPoint.second && move == finalMovement)
            return sides;
        
        // check if can continue in the same direction
        if (!isMovingIntoWall(point, move, lines) && isNextPositionValidSwitched(point, move, nextPosition(point, move), plant, lines))
        {
            unique.insert(nextPosition(point, move));
            return travelInnerPerimeter(nextPosition(point, move), move, startPoint, finalMovement, plant, sides, lines, unique, region);
        }

        // check if can go anti-clockwise instead
        else if (!isMovingIntoWall(point, move, lines) && isNextPositionValidSwitched(nextPosition(point, move), turnAntiClockwise(move), nextPosition(nextPosition(point, move), turnAntiClockwise(move)), plant, lines))
        {
            unique.insert(nextPosition(point, move));
            return travelInnerPerimeter(nextPosition(point, move), turnAntiClockwise(move), startPoint, finalMovement, plant, ++sides, lines, unique, region);
        }
        
        // check if can go clockwise instead
        else if (isNextPositionValidSwitched(point, turnClockwise(move), point, plant, lines))
        {
            // if we're against a wall, this isn't cutting through the region
            if (isMovingIntoWall(point, move, lines))
                return 0;
            return travelInnerPerimeter(point, turnClockwise(move), startPoint, finalMovement, plant, ++sides, lines, unique, region);
        }
        
        // shouldn't reach here
        // as a fallback, assume this means perimeter is non-valid
        else
            return 0;
    }

    template <std::size_t N>
    bool isLandlocked(std::pair<size_t, size_t> point, char plant, const std::array<std::string_view, N>& lines)
    {
        if (point.first == 0 || point.first == lines.at(0).size() - 1 || point.second == 0 || point.second == lines.size() - 1)
            return false;
        return ((lines[point.second - 1][point.first] != plant) && (lines[point.second][point.first + 1] != plant) && (lines[point.second + 1][point.first] != plant) && (lines[point.second][point.first - 1] != plant));
    }

    template <std::size_t N>
    int checkAndGetInnerPerimeters(const PlantRegion& region, const std::array<std::string_view, N>& lines, char plant)
    {
        int innerSide {0};
        // set up set to order by row, instead of column
        PlantRegionByRow regionByRow {};
        for (const std::pair<size_t, size_t>& pair : region)
            regionByRow.insert(pair);
        
        PlantRegion uniquePlants {};
        bool isFirstLoop {true};
        std::pair<size_t, size_t> current {};
        std::pair<size_t, size_t> previous {};
        for (std::pair<size_t, size_t> pair : regionByRow)
        {
            if (isFirstLoop)
            {
                previous = pair;
                isFirstLoop = false;
                continue;
            }
            current = pair;
            // skip if previous was different row, or is consec column
            if (current.second != previous.second || current.first == previous.first + 1)
            {
                previous = pair;
                continue;
            }

            // take space one to right of previous
            std::pair<size_t, size_t> start {previous.first + 1, previous.second};
            std::pair<size_t, size_t> point {start};
            // this shouldn't happen, but just to be sure
            if (lines[start.second][start.first] == plant)
            {
                previous = pair;
                continue;
            }
            // skip if we've already looked at a perimeter here
            // or if this is in the middle of a region
            if (uniquePlants.find(start) != uniquePlants.end() || isLandlocked(start, plant, lines))
            {
                previous = pair;
                continue;
            }
            // skip if a plant type to north is different from the original
            // or if we're on the northern border
            if (start.second == 0 || lines[start.second - 1][start.first] != plant)
            {
                previous = pair;
                continue;
            }
            uniquePlants.insert(start);
            std::pair<int, int> movement {direction::east};
            std::pair<int, int> finalMovement {direction::north};

            innerSide += travelInnerPerimeter(point, movement, start, finalMovement, plant, 1, lines, uniquePlants, region);
        }
        return innerSide;
    }

    template <std::size_t N>
    std::vector<int> calculateSides(const PlantRegions& regions, const std::array<std::string_view, N>& lines, char plant)
    {
        std::vector<int> sides {};
        for (const PlantRegion& region : regions)
        {
            int side {countSides(region, lines, plant)};
            // account for inner perimeters
            side += checkAndGetInnerPerimeters(region, lines, plant);
            sides.push_back(side);
        }
        return sides;
    }

    int calculatePlantPrice(const std::vector<int>& areas, const std::vector<int>& sides)
    {
        int price {0};
        assert((areas.size() == sides.size()) && "Plant does not have the same number of regions for areas and sides.");
        for (size_t i{0}; i < areas.size(); ++i)
            price += areas[i] * sides[i];
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
                    std::vector<int> sides {calculateSides(regions, lines, plant)};
                    int price {calculatePlantPrice(areas, sides)};
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

#endif // AOC12B_H
