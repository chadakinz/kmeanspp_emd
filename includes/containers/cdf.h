#pragma once
#include "numericArray.h"
#include "../config.h"
#include <cstddef>

template <typename T, std::size_T N>
class CDF: NumericArray<T, N>{
private:
    NumericArray<T, N> bin_lower;
public:
    CDF() {
        for (std::size_t i = 0; i < N; i++) {
            bin_lower[i] = static_cast<T>(1) / N;
        }
    }

    PPF<T, PPF_SIZE> get_ppf(){
        PPF<T, PPF_SIZE> ppf;
        std::size_t j = 0;
        T quantile_val = static_cast<T>(1) / PPF_SIZE;
        for(std::size_t i = 0; i < N; i ++){
            while ((*this)[i] >= quantile_val && j < PPF_SIZE){
                ppf[j++] = bin_lower[i];
                quantile_val += static_cast<T>(1) / PPF_SIZE;
            }
            if (j >= PPF_SIZE) break;
        }
        return ppf;
    }
};


