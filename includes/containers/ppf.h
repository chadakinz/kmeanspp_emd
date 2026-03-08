#pragma once
#include "numericArray.h"

template <typename T>
class PPF: public NumericArray<T>{
public:
    PPF(int n) : NumericArray<T>(n){}
};

//Takes two quantile functions and finds the wasserstein distance between them
template <typename T>
T wasserstein_2(PPF<T>& a, PPF<T>& b){
    T sum{};
    for (std::size_t i = 0; i < a.size(); i ++){
        T diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}