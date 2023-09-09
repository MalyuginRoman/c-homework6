#pragma once
#include <map>
#include <algorithm>
#include <tuple>
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
    long x;
    std::pair<long, long> index;
    T element;
    Matrix<T, N>*  matrixPtr;
};

template<class T, T N>
class Matrix{
    using IteratorType = typename std::map<std::pair<long, long>, T>::iterator;
public:
    Matrix() : defaultElement(N)
    {};
    ~Matrix(){}
    Proxy<T, N> operator[](long index){
        Proxy<T, N> proxy(this, index);
        return proxy;
    };
    size_t size(){
        return matrix.size();
    }
    T getDefaultElement(){
        return defaultElement;
    };
    //поиск элемента
    const T& findIndex(std::pair<long, long> index) const {
        auto it = matrix.find(index);
        if( it == matrix.end()){
            return defaultElement;
        }
        else{
            return it->second;
        }
    };
    //удаление элемента
    void removeElement(std::pair<long, long> index){
        matrix.erase(matrix.find(index));
    };
    //вставка элемента
    void insertElement(std::pair<long, long> index, T value){
        matrix.emplace(index, value);
    };
    //итератор
    class Iterator{
    public:
        Iterator(IteratorType  matrixIt_):iterator(matrixIt_){};
        IteratorType getInsideIterator(){
            return this->iterator;
        }
        void operator++(){
            iterator++;
        };
        std::tuple<long, long, T> operator*(){
            return std::make_tuple(iterator->first.first, iterator->first.second, iterator->second);
        };
        void operator--(){
            iterator--;
        }
        bool operator !=(Iterator& other){
            return iterator != other.getInsideIterator();
        }
        bool operator ==(Iterator& other){
            return iterator == other.getInsideIterator();
        }
    private:
        IteratorType  iterator;
    };
    Iterator begin(){
        return Iterator(matrix.begin());
    };
    Iterator end(){
        return Iterator(matrix.end());
    };
private:
    const T defaultElement;
    std::map<std::pair<long, long>, T > matrix;
};

int version();
