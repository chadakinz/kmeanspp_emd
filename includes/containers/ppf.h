#pragma once
#include "numericArray.h"
#include <cmath>
#include "cdf.h"
template <typename T>
class CDF;

template <typename T>
class PPF: public NumericArray<T>{
private:
    inline int get_index_pdf(T val, int cdf_size){
        return (2*val*cdf_size - 1)/2;
    }
public:
    PPF() : NumericArray<T>(){}
    PPF(int n) : NumericArray<T>(n){}
    PPF(std::initializer_list<T> values): NumericArray<T>(values.size()){
        int i = 0;
        for(T val: values){
            (*this)[i] = val;
            i++;
        }
    }
    bool operator==(const PPF<T>& other) const {
        return static_cast<const NumericArray<T>&>(*this) == static_cast<const NumericArray<T>&>(other);
    }
    PPF(const NumericArray<T>& arr) : NumericArray<T>(arr.size()) {
        for(size_t i = 0; i < arr.size(); i++)
            (*this)[i] = arr[i];
    }
    CDF<T> get_cdf(int cdf_size){
        T ppf_accum = static_cast<T>(1)/(*this).size();
        T val{};
        T placeholder{};
        int j;
        CDF<T> cdf(cdf_size);
        for(int i = 0; i < (*this).size() - 1; i++){
            val += ppf_accum;
            if ((*this)[i] != (*this)[i+1]){
                j = get_index_pdf((*this)[i], cdf_size);
                //std::cout << val << std::endl;
                cdf[j] = val;
                placeholder = val;
            }
        }
        val += ppf_accum;
        j = get_index_pdf((*this)[(*this).size()-1], cdf_size);
        cdf[j] = val;
        T val2{};
        for(int i = 0; i <cdf_size; i++){
            if(cdf[i] == 0){
                cdf[i] = val2;
            }else{
                val2 = cdf[i];
            }
        }
        return cdf;
    }
    PPF<T>& operator/=(T n){
        for(int i = 0; i < this->size(); i++){
            (*this)[i] /= n;
        }
        return *this;
    }
    PPF<T>& operator += (PPF<T> a){
        for(int i = 0; i < this->size(); i++){
            (*this)[i] += a[i];
        }
        return *this;
    }

};

//Takes two quantile functions and finds the wasserstein distance between them

template<typename T>
T wasserstein_2(PPF<T>& a, PPF<T>& b){
    T sum{};
    std::size_t N = a.size();
    for (std::size_t i = 0; i < N; i++){
        T diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum / N);  // normalize by number of bins and take sqrt
}
