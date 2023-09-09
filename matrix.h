#pragma once
#include <map>
#include <algorithm>
#include <tuple>
#include <memory>

#include "lib.h"

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
        Iterator(IteratorType  Iter):iterator(Iter){};
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
