#ifndef MAZE_H
#define MAZE_H

#include <array>
#include <string>
#include <string_view>
#include <stdexcept>
#include "../../shared/Direction.h"

template <std::size_t N>
class Maze
{
private:
    std::array<std::string, N> m_maze {};
    std::pair<size_t, size_t> m_start {};
    std::pair<size_t, size_t> m_end {};

public:
    Maze(const std::array<std::string_view, N>& maze)
        : m_maze {makeString(maze)}
        , m_start {findPos('S')}
        , m_end {findPos('E')}
    {};

    std::array<std::string, N>& getMaze() { return m_maze; }
    std::string_view getRow(size_t index) { return m_maze[index]; }
    const std::pair<size_t, size_t> getStart() const { return m_start; }
    const std::pair<size_t, size_t> getEnd() const { return m_end; }
    const char getPos(size_t x, size_t y) const { return m_maze[y][x]; }

    void fillMaze(size_t x, size_t y) { m_maze[y][x] = '#'; }

    std::array<std::string, N> makeString(const std::array<std::string_view, N>& maze)
    {
        std::array<std::string, N> m_maze {};
        size_t index {0};
        for (std::string_view row : maze)
        {
            m_maze[index] = std::string(row);
            ++index;
        }
        return m_maze;
    }

    const std::pair<size_t, size_t> findPos(char c) const
    {
        size_t y {0};
        for (std::string_view row : m_maze)
        {
            auto pos {row.find(c)};
            if (pos != std::string_view::npos)
                return {pos, y};
            ++y;
        }
        throw std::invalid_argument("Could not find position.\n");
    }

    const int countWalls(size_t x, size_t y) const
    {
        int count {0};
        for (std::pair<int, int> move : Direction::allDirections)
        {
            if (m_maze[y + move.second][x + move.first] == '#')
                ++count;
        }
        return count;
    }

    void removeDeadEnds()
    {
        int count {1};
        while (count > 0)
        {
            size_t y {0};
            count = 0;
            for (std::string& row : m_maze)
            {
                for (size_t x{1}; x < row.size() - 1; ++x)
                {
                    // dead end if position has 3 walls
                    if (row[x] == '.' && countWalls(x, y) >= 3)
                    {
                        row[x] = '#';
                        ++count;
                    }
                }
                ++y;
            }
        }
    }
};

#endif // MAZE_H
