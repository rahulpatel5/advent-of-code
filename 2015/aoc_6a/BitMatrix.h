// I wanted to create a dynamic class that could be any size
// but there are limitations with using std::bitset for this.
// Setting up this fixed length class for this problem only

#ifndef BITMATRIX_H
#define BITMATRIX_H

#include <vector>
#include <bitset>

class BitMatrix
{
private:
    int m_rows{};
    std::vector<std::bitset<1000>> m_matrix{};

public:
    BitMatrix(int rows);

    const std::vector<std::bitset<1000>>& getMatrix() const;

    void setRowSET(std::size_t index, std::bitset<1000> mask);
    void setRowXOR(std::size_t index, std::bitset<1000> mask);
    void setRowNOT(std::size_t index, std::bitset<1000> mask);
};

int countActiveLights(const BitMatrix& matrix);

#endif // BITMATRIX_H
