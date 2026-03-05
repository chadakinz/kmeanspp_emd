#pragma once
#include "numericArray.h"
#include "cdf.h"
#include <cstddef>
#include <string>

template <typename T, std::size_t N>
class PDF: public NumericArray<T, N>{
public:
    CDF<T,N> get_cdf() const{
        CDF<T, N> cdf;
        T sum{};
        for (std::size_t i = 0; i < N; i++){
            sum += (*this)[i];
            cdf[i] = sum;
        }
        return cdf;
    }
};

template<typename T, std::size_t N>
private inline PDF<T, N> process_line_into_pdf(std::string line){
    PDF<T, N> pdf;
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


