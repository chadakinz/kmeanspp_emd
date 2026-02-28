#include <vector>
#include <iostream>
#include "wkmeans.h"
#include <utility>
#include <algorithm>
#include <array>
#include "distributions.h"

using namespace pkmeans;
inline std::string get_string(char*argv, int& i){
    std::string s = "";
    while(argv[i] != ' ' && argv[i] != '\0'){
        s += argv[i++];
    }
    return s;
}

void process_input(int argc, char* argv, int& num_clusters,  float& epsilon, std::string& output_file,
                      std::string& input_file){
    int i = 0;
    std::string value;
    while(argv[i] != '\0'){
        if (argv[i] == 'k') {
            i += 2;
            value = get_string(argv, i);
            num_clusers = stoi(value);
        }
        else if (argv[i] == 's'){
            i +=2;
            value = get_string(argv, i);
            epsilon = std::stof(value);

        }else if(argv[i] == 'o'){
            i += 2;
            value = get_string(argv, i);
            output_file = value;
        }
        else if(argv[i] == 'i'){
            i += 2;
            value = get_string(argv, i);
            input_file = value;
        }
           i++;
    }
}

std::pair<int,int> get_dimensions(const std::string& file_name) {
    std::ifstream file(file_name);

    std::string first_line;
    std::getline(file, first_line);

    int columns = std::count(first_line.begin(), first_line.end(), ',') + 1;

    int rows = 1 + std::count(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>(),
            '\n'
    );

    return {rows, columns};
}

int main(int argc, char* argv){
    int num_clusters = 10;
    float epsilon = 1e-4;
    int feature, d_size;
    std::string output_file = "output.csv";
    std::string input_file;

    if (argc == 1){
        std::cout << "(flag, default): description" << std::endl;
        std::cout << "(-k, 10): number of clusters" << std::endl;
        std::cout << "(-i, NONE): name of the input file (must be a csv)" << std::endl;
        std::cout << "(-o, output.csv): name of the output file"
        std::cout << "(-s, 1e-4): epsilon parameter used to specify when program should terminate" << std::endl;
    }
    process_input(argc, argv, num_clusters, epsilon, output_file, input_file);
    feature, d_size = get_dimensions(input_file);
    std::vector<Distribution<float, feature>> pdfs;
    std::vector<Distribution<float, feature>> cdfs;
    pdfs.reserve(d_size);
    cdfs.reserve(d_size);
    initialize_distributions(input_file, pdfs, cdfs);
    WKmeans<Distribution<float, feature>> wkmeans(d_size, feature, epsilon, pdfs, cdfs);

    return 0;
}

