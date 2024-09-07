#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

class Matrix
{
private:
    std::vector<std::vector<int>> m_matrix{};

public:
    Matrix(int rows, int columns);

    const std::vector<std::vector<int>>& getMatrix() const;

    void addToMatrix(std::size_t index, std::size_t start, std::size_t end, int value);
    void reduceMatrix(std::size_t index, std::size_t start, std::size_t end);
};

int countActiveLights(const Matrix& matrix);

#endif // MATRIX_H
