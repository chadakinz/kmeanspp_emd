#pragma once
#include "numericArray.h"
#include "../config.h"
#include <cstddef>
#include "./ppf.h"


template <typename T>
class CDF: public NumericArray<T>{
private:
    NumericArray<T> bin_lower;
public:
    CDF(int n): NumericArray<T>(n), bin_lower(n){
        for (std::size_t i = 0; i < (*this).size(); i++) {
            bin_lower[i] = static_cast<T>(1) / (*this).size();
        }
    }

    PPF<T> get_ppf(){
        PPF<T> ppf(PPF_SIZE);
        std::size_t j = 0;
        T quantile_val = static_cast<T>(1) / PPF_SIZE;
        for(std::size_t i = 0; i < (*this).size(); i ++){
            while ((*this)[i] >= quantile_val && j < PPF_SIZE){
                ppf[j++] = bin_lower[i];
                quantile_val += static_cast<T>(1) / PPF_SIZE;
            }
            if (j >= PPF_SIZE) break;
        }
        return ppf;
    }
};


