#include <array>
#include <vector>
#include <utility>
#include "distributions.h"


//EMD implementation where inputs are assumed to be CDFs
template<typename T, std::size_t N>
T emd(const Distribution<T, N>& x, const Distribution<T,N>& y){
    return x.abs_diff(y).sum();
}

template<typename T, std::size_t N>
private inline Distribution<T, N> process_line_into_pdf(std::string line){
    Distribution<T, N> pdf;
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

template<typename T, std::size_T N>
void init_distributions(std::string file_name, Distribution<T, N>& pdfs, Distribution<T, N>& cdfs){
    std::ifstream file(file_name);
    std::string line;
    Distribution<T, N> temp_pdf;
    while(std::getline(file, line)){
        temp_pdf = process_line_into_pdf(line);
        pdfs.push_back(temp_pdf);
        cdfs.push_back(temp_pdf.cum_sum());
    }
}