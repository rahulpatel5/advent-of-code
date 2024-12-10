#include "Matrix.h"

Matrix::Matrix(int rows, int columns)
    : m_matrix(rows, std::vector<int>(columns))
    {}

const std::vector<std::vector<int>>& Matrix::getMatrix() const
{
    return m_matrix;
}

void Matrix::addToMatrix(const std::size_t index, const std::size_t start, const std::size_t end, const int value)
{
    for (auto i{start}; i <= end; ++i)
    {
        m_matrix[index][i] += value;
    }
}

void Matrix::reduceMatrix(const std::size_t index, const std::size_t start, const std::size_t end)
{
    for (auto i{start}; i <= end; ++i)
    {
        if (m_matrix[index][i] > 0)
            m_matrix[index][i] -= 1;
    }
}

int countActiveLights(const Matrix& matrix)
{
    int onLights{0};
    for (const auto& row : matrix.getMatrix())
    {
        for (const auto& column : row)
        {
            onLights += column;
        }
    }
    return onLights;
}
