#pragma once

#include "matrix.h"
#include <memory>

template<class T, T N>
class Matrix;

template<class T, T N>
class Proxy{
public:
    Proxy(Matrix<T, N> * matrixPtr_, long x_) :
    matrixPtr(matrixPtr_), element(0), x(x_) {};
    ~Proxy(){};
    Proxy& operator[](int y){
        index = std::make_pair(x, y);
        element = matrixPtr->findIndex(index);
        return *this;
    };
    void operator=(T newValue){
        if(newValue != matrixPtr->getDefaultElement()){
            if(element != matrixPtr->getDefaultElement()){
                element = newValue;
            }
            else{
                matrixPtr->insertElement(index, newValue);
            }
        }
        else{
            if(element != matrixPtr->getDefaultElement()){
                matrixPtr->removeElement(index);
                return;
            }
            else return;
        }
    };
    operator T(){
        return element;
    }
private:
    T element;
    std::pair<long, long> index;
    Matrix<T, N>*  matrixPtr;
    long x;
};
