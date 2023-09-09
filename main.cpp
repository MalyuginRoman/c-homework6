#include <iostream>
#include <string>
#include <tuple>

#include "lib.h"
#include "matrix.h"

int main(int, char const **) {

    Matrix<int, 0> matrix;

    for (auto i = 0; i < 10; i++) {
        matrix[i][i] = i;
        matrix[i][9 - i] = 9 - i;
    }

    for(auto i = 1; i < 9; i++){
        for (auto j = 1; j < 9; j++){
            std::cout << matrix[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << "Busy cells = " << matrix.size() << std::endl;

    for(auto c : matrix){
        int x, y, v;
        std::tie(x, y, v) = c;
        std::cout << x << " " << y << " " << v << std::endl;
    }
  return 0;
}
