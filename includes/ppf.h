#pragma once
#include "numericArray.h"
#include "cdf.h"
#include "pdf.h"

template <typename T, std::size_t N>
class PPF: public NumericArray<T, N>{
public:
    T wasserstein_2()
};

//Takes two quantile functions and finds the wasserstein distance between them
template <typename T, std::size_t N>
T wasserstein_2(PPF<T, N>& a, PPF<T, N>& b){
    T sum{};
    for (std::size_t i = 0; i < N; i ++){
        T diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}