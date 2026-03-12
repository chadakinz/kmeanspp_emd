#pragma once
#include <array>
#include <vector>
#include <cstddef>
#include <iostream>

template<typename T>
class NumericArray {
private:
    std::vector<T> _data;
    int N;

public:
    NumericArray(int n) : N(n) {_data.resize(n);}

    T& operator[](std::size_t i){ return _data[i]; }
    const T& operator[](std::size_t i) const {return _data[i]; }

    bool operator==(const NumericArray& b) const{
        bool bool1 = true;
        for(int i = 0; i < b.size(); i++){
            bool1 &= (*this)[i] == b[i];
        }
        return bool1;
    }

    friend std::ostream& operator<<(std::ostream& os, const NumericArray<T>& a){
        os << "[";
        for (std::size_t i = 0; i < a.size(); i++){
            os << a[i];
            if (i != a.size() - 1) os << ", ";
        }
        os << "]";
        return os;
    }

    NumericArray operator+(const NumericArray& b){
        NumericArray x(N);
        for(int i = 0; i < N; i++){
            x[i] = _data[i] + b[i];
        }
        return x;
    }

    NumericArray operator-(const NumericArray& b){
        NumericArray x(N);
        for(int i = 0; i < N; i++){
            x[i] = _data[i] - b[i];
        }
        return x;
    }

    T sum(){
        T val{};
        for(int i = 0; i < N; i++){
            val += _data[i];
        }
        return val;
    }

    void normalize(){
        T val = sum();
        for(int i = 0; i < N; i++){
            _data[i] /= val;
        }
    }
    NumericArray cum_sum(){
        NumericArray c(N);
        T val{};
        for(int i = 0; i < N; i++){
            val += _data[i];
            c[i] = val;
        }
        return c;
    }
    std::size_t size() const {return N;}

    void print(){
        for(int i = 0; i<N; i++){
            std::cout << _data[i] << " ";
        }
        std::cout << std::endl;
    }
    auto begin() { return _data.begin(); }
    auto end() { return _data.end(); }
    auto begin() const { return _data.begin(); }
    auto end() const { return _data.end(); }
};