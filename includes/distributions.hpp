#pragma once
#include <array>
#include <vector>
#include <utility>
#include "distributions.h"

template<typename T>
void init_distributions(const std::string& file_name, std::vector<PDF<T>>& pdfs,
                        std::vector<CDF<T>>& cdfs,  std::vector<PPF<T>>& ppfs, int features){
    std::ifstream file(file_name);
    std::string line;
    PDF<T> temp_pdf(features);
    CDF<T> temp_cdf(features);
    while(std::getline(file, line)){
        temp_pdf = process_line_into_pdf(line, features);
        pdfs.push_back(temp_pdf);
        temp_cdf = temp_pdf.get_cdf();
        cdfs.push_back(temp_cdf);
        ppfs.push_back(temp_cdf.get_ppf());
    }
}