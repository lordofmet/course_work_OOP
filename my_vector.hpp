/*#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#endif // MY_VECTOR_HPP

#include "my_vector.h"

template <typename T>
arr<T>::arr() {
    minInd = 0;
    maxInd = 100;
    size = maxInd - minInd + 1;
    a = new T[size];
    cur = 0;
}

template <typename T>
arr<T>::arr(const arr& ar) {
    minInd = ar.minInd;
    maxInd = ar.maxInd;
    size = ar.size;
    a = new T[size];
    for (int i = 0; i < size; i++) a[i] = ar.a[i];
    cur = ar.cur;
}

template <typename T>
arr<T>::arr(int _minInd, int _maxInd) {
    minInd = _minInd;
    maxInd = _maxInd;
    size = maxInd - minInd + 1; //уже нельзя расширить
    a = new T[size];
    cur = 0;
}

template <typename T>
int arr<T>::get_size() {
    return cur;
}

template <typename T>
void arr<T>::copy(arr& n) {
    n.minInd = minInd;
    n.maxInd = maxInd;
    n.size = size;
    n.cur = cur;
    delete[] n.a;
    n.a = new T[size];
    for (int i = 0; i < cur; i++) n.a[i] = a[i];
}

template <typename T>
void arr<T>::set(int& _minInd, int& _maxInd, int& _size, T*& _a, int& _cur) {
    minInd = _minInd;
    maxInd = _maxInd;
    size = _size;
    delete[] a;
    a = _a; //уже выделенная память из кучи
    cur = _cur;
}

template <typename T>
void arr<T>::read() {
    for (int i = 0; i < size; i++) {
        cin >> a[i];
    }
    cur = size;
}

template <typename T>
void arr<T>::read(int n) {
    if (n <= size) {
        cur = max(n, cur);
        for (int i = 0; i < n; i++) {
            cin >> a[i];
        }
    }
    else {
        if (n > maxInd - minInd + 1) {
            cout << "Cant add\n";
            return;
        }
        T* new_a = new T[n];
        for (int i = 0; i < cur; i++)
            new_a[i] = a[i];
        delete[] a;
        a = new_a;
        cur = n;
        size = n;
        for (int i = 0; i < n; i++) {
            cin >> a[i];
        }
    }
}

template <typename T>
void arr<T>::print() {
    cout << "size = " << size << ": \n";
    for (int i = 0; i < cur; i++) {
        cout << "arr[" << i + minInd << "] = " << a[i] << "\n";
    }
}

template <typename T>
T* arr<T>::begin() {
    if (cur)
        return a;
    return nullptr;
}

template <typename T>
void arr<T>::add(T val) {
    if (cur == maxInd - minInd + 1) {
        cout << "Cant add more\n";
        return;
    }
    if (cur < size) {
        a[cur] = val;
    }
    else {
        if (2 * size - 1 + minInd <= maxInd) //пытаемся выделить size * 2
            size *= 2;
        else
            size = maxInd - minInd + 1;
        T* new_a = new T[size];
        for (int i = 0; i < cur; i++)
            new_a[i] = a[i];
        delete[] a;
        a = new_a;
        a[cur] = val;
    }
    cur++;
}

template <typename T>
arr<T>::~arr() {
    delete[] a;
}
*/
