// uses code on learncpp website
// https://www.learncpp.com/cpp-tutorial/multidimensional-stdarray/

#ifndef ARRAY_H
#define ARRAY_H

#include <array>

namespace Array
{
    template <typename T, std::size_t Row, std::size_t Col>
    using Array2d = std::array<std::array<T, Col>, Row>;

    template <typename T, std::size_t Row, std::size_t Col>
    constexpr std::size_t rowLength(const Array2d<T, Row, Col>& arr)
    {
        return Row;
    }

    template <typename T, std::size_t Row, std::size_t Col>
    constexpr int rowLength(const Array2d<T, Row, Col>& arr)
    {
        return Row;
    }

    template <typename T, std::size_t Row, std::size_t Col>
    constexpr std::size_t colLength(const Array2d<T, Row, Col>& arr)
    {
        return Col;
    }

    template <typename T, std::size_t Row, std::size_t Col>
    constexpr int colLength(const Array2d<T, Row, Col>& arr)
    {
        return Col;
    }
}

#endif // ARRAY_H
