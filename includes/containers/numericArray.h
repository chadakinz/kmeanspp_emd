#pragma once
#include <array>
#include <vector>
#include <cstddef>

template<typename T>
class NumericArray {
private:
    std::vector<T> _data;
    int N;

public:
    NumericArray(int n) : N(n) {_data.resize(n);}


    T& operator[](std::size_t i){ return _data[i]; }
    const T& operator[](std::size_t i) const {return _data[i]; }

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
};