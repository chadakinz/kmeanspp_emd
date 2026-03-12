#pragma once
#include "numericArray.h"
#include "cdf.h"
#include <cstddef>
#include <string>
#include <iostream>

template <typename T>
class CDF;

template <typename T>
class PDF: public NumericArray<T>{
public:
    PDF(int n): NumericArray<T>(n){}

    PDF(std::initializer_list<T> values): NumericArray<T>(values.size()){
        int i = 0;
        for(T val: values){
            (*this)[i] = val;
            i++;
        }
    }

    CDF<T> get_cdf() const {
        CDF<T> cdf((*this).size());
        T sum{};
        for (std::size_t i = 0; i < (*this).size(); i++){
            sum += (*this)[i];
            cdf[i] = sum;
        }
        return cdf;
    }
};

template<typename T>
inline PDF<T> process_line_into_pdf(std::string line, int n){
    PDF<T> pdf(n);
    int count = 0;
    std::string f_val = "";
    for(int i = 0; i < line.size(); i++){

        if(line[i] == ','){
            pdf[count++] = std::stof(f_val);
            f_val = "";
        }
        f_val += line[i];
    }
    return pdf.normalize();
}


