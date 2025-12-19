#ifndef AOC9B_H
#define AOC9B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <ranges>
#include <stdexcept>

/*
FOR each pair of points:
    FIND area
    IF area could be bigger than current biggest:
        IF if each point would be red/green tile AND no 'cut-ins':
            STORE as the biggest area

RETURN biggest area

on 'cut-ins':
can't just look at corners
red/green tile could cut into the area, so that the rectangle isn't valid
look at whether any corners exist within the rectangle area
this assumes that there are no unconnected corners or straight lines

use information about consecutive points being connected
to identify when there is an actual cut-in
stop the check that the rectangle is encompassed
since we can rely on cut-ins to identify where that's not the case
assume that there's no overlap in lines connecting points,
and assume no adjacent lines, i.e. any cut-in is a true cut-in
assume that points are at 90 degrees, i.e. not continuing straight line

missing something for the solution
tested and confirmed no overlapping lines, so don't worry about those
after testing, I think one issue in the code is a false negative
(counting as a cut-in) when a line overlaps with an edge, but
isn't a corner. Handle this and see if it gives the right answer
*/

namespace aoc9b
{
    using AreaInt = long long;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Point = std::pair<std::size_t, std::size_t>;
    using Points = std::vector<Point>;
    using PointMap = std::map<std::size_t, std::vector<std::size_t>>;

    template <std::size_t L>
    void readPoints(const Lines<L>& lines, Points& points, PointMap& colPointMap, PointMap& rowPointMap)
    {
        for (std::string_view line : lines)
        {
            size_t comma { line.find(',') };
            std::string numOneStr { line.substr(0, comma) };
            std::string numTwoStr { line.substr(comma + 1) };
            AreaInt numOne { std::stoll(numOneStr) };
            AreaInt numTwo { std::stoll(numTwoStr) };

            points.push_back(std::make_pair(numOne, numTwo));
            colPointMap[numOne].push_back(numTwo);
            rowPointMap[numTwo].push_back(numOne);
        }
    }

    AreaInt getArea(const Point& point1, const Point& point2)
    {
        return (std::llabs(point2.first - point1.first) + 1) * (std::llabs(point2.second - point1.second) + 1);
    }

    // checkGreater: true if checking more than checkPos, false if less
    // checkRow: true if row is changing, false if column is changing
    bool isCrossing(size_t col, size_t row, size_t checkPos, bool checkGreater, bool checkRow, const Points& points)
    {
        Point curr {std::make_pair(col, row)};
        auto currIter { std::find(points.begin(), points.end(), curr) };
        size_t currIndex { static_cast<std::size_t>(std::distance(points.begin(), currIter)) };
        
        Point prev {};
        if (curr == points.front())
            prev = points.back();
        else
            prev = points.at(currIndex - 1);
        Point next {};
        if (curr == points.back())
            next = points.front();
        else
            next = points.at(currIndex + 1);
        
        Point check {};
        if (checkRow)
        {
            if (curr.first == prev.first)
                check = prev;
            else if (curr.first == next.first)
                check = next;
            else
                return false;
        }
        else
        {
            if (curr.second == prev.second)
                check = prev;
            else if (curr.second == next.second)
                check = next;
            else
                return false;
        }
        
        if (checkGreater)
        {
            if (checkRow)
            {
                if (check.second >= checkPos)
                    return true;
            }
            else
            {
                if (check.first >= checkPos)
                    return true;
            }
        }
        else
        {
            if (checkRow)
            {
                if (check.second <= checkPos)
                    return true;
            }
            else
            {
                if (check.first <= checkPos)
                    return true;
            }
        }

        return false;
    }

    bool isExtensionOfEdge(size_t col, size_t row, const Points& corners, size_t minCol, size_t maxCol, size_t minRow, size_t maxRow)
    {
        for (const Point& corner : corners)
        {
            // want a point in line with the edge (line segment)
            // but not within the edge
            if ((corner.first == col && (row < minRow || row > maxRow)) || (corner.second == row && (col < minCol || col > maxCol)))
                return true;
        }
        return false;
    }

    bool isCutIn(const Point& point1, const Point& point2, const PointMap& colPointMap, const PointMap& rowPointMap, const Points& points)
    {
        size_t minCol { point1.first < point2.first ? point1.first : point2.first };
        size_t maxCol { minCol == point1.first ? point2.first : point1.first };
        size_t minRow { point1.second < point2.second ? point1.second : point2.second };
        size_t maxRow { minRow == point1.second ? point2.second : point1.second };

        Points corners {
            point1,
            point2,
            std::make_pair(point1.first, point2.second),
            std::make_pair(point2.first, point1.second)
        };

        for (const auto& [col, rows] : colPointMap)
        {
            if (col <= minCol)
            {
                for (size_t row : rows)
                {
                    if (row >= minRow && row <= maxRow && std::find(corners.begin(), corners.end(), std::make_pair(col, row)) == corners.end() && !isExtensionOfEdge(col, row, corners, minCol, maxCol, minRow, maxRow) && isCrossing(col, row, maxCol, true, false, points))
                    {
                        return true;
                    }
                }
            }
            else if (col >= maxCol)
            {
                for (size_t row : rows)
                {
                    if (row >= minRow && row <= maxRow && std::find(corners.begin(), corners.end(), std::make_pair(col, row)) == corners.end() && !isExtensionOfEdge(col, row, corners, minCol, maxCol, minRow, maxRow) && isCrossing(col, row, minCol, false, false, points))
                    {
                        return true;
                    }
                }
            }
            // if any point is within the rectangle, this is a cut-in
            else
            {
                for (size_t row : rows)
                {
                    if (row > minRow && row < maxRow)
                    {
                        return true;
                    }
                }
            }
        }

        for (const auto& [row, cols] : rowPointMap)
        {
            if (row <= minRow)
            {
                for (size_t col : cols)
                {
                    if (col >= minCol && col <= maxCol && std::find(corners.begin(), corners.end(), std::make_pair(col, row)) == corners.end() && !isExtensionOfEdge(col, row, corners, minCol, maxCol, minRow, maxRow) && isCrossing(col, row, maxRow, true, true, points))
                    {
                        return true;
                    }
                }
            }
            else if (row >= maxRow)
            {
                for (size_t col : cols)
                {
                    if (col >= minCol && col <= maxCol && std::find(corners.begin(), corners.end(), std::make_pair(col, row)) == corners.end() && !isExtensionOfEdge(col, row, corners, minCol, maxCol, minRow, maxRow) && isCrossing(col, row, minRow, false, true, points))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    template <std::size_t L>
    AreaInt getLargestArea(const Lines<L>& lines)
    {
        Points points {};
        // column is key, rows are values
        PointMap colPointMap {};
        // row is key, columns are values
        PointMap rowPointMap {};
        readPoints<L>(lines, points, colPointMap, rowPointMap);

        AreaInt biggestArea {0};
        for (size_t point1{0}; point1 < points.size(); ++point1)
        {
            for (size_t point2{point1 + 1}; point2 < points.size(); ++point2)
            {
                if (getArea(points.at(point1), points.at(point2)) > biggestArea && !isCutIn(points.at(point1), points.at(point2), colPointMap, rowPointMap, points))
                {
                    biggestArea = getArea(points.at(point1), points.at(point2));
                }
            }
        }

        return biggestArea;
    }
}

#endif // AOC9B_H
