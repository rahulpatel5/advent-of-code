#include "BitMatrix.h"

BitMatrix::BitMatrix(int rows)
    : m_rows{rows}
    , m_matrix(rows)
    {}

const std::vector<std::bitset<1000>>& BitMatrix::getMatrix() const
{
    return m_matrix;
}

void BitMatrix::setRowSET(std::size_t index, std::bitset<1000> mask)
{
    m_matrix[index] |= mask;
}

void BitMatrix::setRowXOR(std::size_t index, std::bitset<1000> mask)
{
    m_matrix[index] ^= mask;
}

void BitMatrix::setRowNOT(std::size_t index, std::bitset<1000> mask)
{
    m_matrix[index] &= ~mask;
}

int countActiveLights(const BitMatrix& matrix)
{
    int onLights{0};
    for (const auto& row : matrix.getMatrix())
    {
        onLights += row.count();
    }
    return onLights;
}
