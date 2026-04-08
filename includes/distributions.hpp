#pragma once
#include <array>
#include <vector>
#include <utility>
#include <fstream>
#include "containers/pdf.h"

//TODO: provide implementation for csv formatted files
template <typename T>
void process_line_into_pdf(const std::string& line, int features, PDF<T>& pdf){
    std::string number = "";
    int count = 0;
    for(int i = 0; i < line.size(); i++){
        if(line[i] == ' '){
            pdf[count++] = std::stof(number);
            number = "";
            continue;
        }
        number += line[i];
    }

    if (count < features){
        pdf[features - 1] = std::stof(number);
    }
}

template<typename T>
void init_distributions(const std::string& file_name, std::vector<PDF<T>>& pdfs,
                        std::vector<CDF<T>>& cdfs,  std::vector<PPF<T>>& ppfs, int features){
    std::ifstream file(file_name);
    std::string line;
    CDF<T> temp_cdf(features);
    PDF<T> temp_pdf(features);
    while(std::getline(file, line)){
        process_line_into_pdf(line, features, temp_pdf);
        temp_pdf.normalize();
        pdfs.push_back(temp_pdf);
        temp_cdf = temp_pdf.get_cdf();
        cdfs.push_back(temp_cdf);
        ppfs.push_back(temp_cdf.get_ppf());
    }
}