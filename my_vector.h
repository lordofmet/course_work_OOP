#pragma once
#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

template <typename T>
class arr {
    int minInd, maxInd;
    int cur; //cur - первый незаполненный элемент(индекс)
    int size; //size - на сколько элементов выделено памяти (max = maxInd - minInd + 1)
    T* a;
public:
    arr();

    arr(const arr& ar);

    int get_size();

    void copy(arr& n);

    void set(int& _minInd, int& _maxInd, int& _size, T*& _a, int& _cur);

    void read();

    void read(int n);

    void print();

    void add(T val);

    T* begin();

    T* end();

    T& operator[] (int ind);

    ~arr();
};

#endif // MY_VECTOR_H
