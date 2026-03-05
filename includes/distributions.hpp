#pragma once
#include <array>
#include <vector>
#include <utility>
#include "distributions.h"

template<typename T, std::size_T N>
void init_distributions(const std::string& file_name, std::vector<PDF<float, feature>>& pdfs,
                        std::vector<CDF<float, feature>>& cdfs,  std::vector<PPF<float, feature>>& ppfs){
    std::ifstream file(file_name);
    std::string line;
    PDF<T, N> temp_pdf;
    CDF<T, N> temp_cdf;
    while(std::getline(file, line)){
        temp_pdf = process_line_into_pdf(line);
        pdfs.push_back(temp_pdf);
        temp_cdf = temp_pdf.get_cdf();
        cdfs.push_back(temp_cdf);
        ppfs.push_back(temp_cdf.get_ppf());
    }
}