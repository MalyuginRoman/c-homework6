#include <iostream>

#include "lib.h"

int main(int, char const **) {
    
	Matrix<int, 2, 0> matrix;

    for (size_t i = 0; i < 10; ++i) {
        matrix[i][i] = i;
        matrix[i][9 - i] = 9 - i;
    }
    for (size_t i = 1; i <= 8; ++i) {
        for (size_t j = 1; j <= 8; ++j) {
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "Busy cells = " << matrix.size() << std::endl;
    for (auto occupied : matrix) {
        for (auto index : occupied.first)
            std::cout << index << ' ';
        std::cout << occupied.second << std::endl;
    }

    return 0;
}