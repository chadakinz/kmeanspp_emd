#pragma once
#include "numericArray.h"
#include "../config.h"
#include <cstddef>
#include "./ppf.h"

template <typename T>
class PPF;

template <typename T>
class CDF: public NumericArray<T>{
private:
    NumericArray<T> bin_lower;
public:
    CDF(int n): NumericArray<T>(n), bin_lower(n){
        for (std::size_t i = 0; i < (*this).size(); i++) {
            (*this)[i] = 0;
            bin_lower[i] = ((static_cast<T>(i) / (*this).size()) + (static_cast<T>(i + 1) / (*this).size()))/2;
        }
    }

    CDF(std::initializer_list<T> values): NumericArray<T>(values.size()), bin_lower(values.size())
    {
        std::size_t i = 0;
        for(T v: values){
            (*this)[i] = v;
            bin_lower[i] = (static_cast<T>(i))/(*this).size();
            i++;
        }
    }

    PPF<T> get_ppf(){
        PPF<T> ppf(PPF_SIZE);
        std::size_t j = 0;
        T quantile_val = static_cast<T>(1) / PPF_SIZE;
        for(std::size_t i = 0; i < (*this).size(); i ++){
            while ((*this)[i] >= quantile_val - .001){
                ppf[j++] = bin_lower[i];
                quantile_val += static_cast<T>(1) / PPF_SIZE;
            }
            if (j > PPF_SIZE) break;
        }
        return ppf;
    }
};


