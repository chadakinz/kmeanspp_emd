#include <vector>
#include <iostream>
#include "wkmeans.h"
#include <utility>
#include <algorithm>
#include <array>
#include "distributions.h"
#include "config.h"

using namespace pkmeans;
void process_input(int argc, char* argv[], Config& cfg){
    int i = 0;
    std::string value;
    for(int i = 0; i < argc; i++){
        if (argv[i] == "-k") {
            cfg.num_clusters = stoi(argv[++i]);
        }
        else if (argv[i] == "-s"){
            cfg.epsilon = std::stof(argv[++i]);

        }else if(argv[i] == "-o"){
            cfg.output_file = argv[++i];
        }
        else if(argv[i] == "-i"){

            cfg.input_file = argv[++i];
        }
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

int main(int argc, char* argv[]){
    Config cfg;
    int feature, d_size;
    std::string output_file = "output.csv";


    if (argc == 1){
        std::cout << "(flag, default): description" << std::endl;
        std::cout << "(-k, 10): number of clusters" << std::endl;
        std::cout << "(-i, NONE): name of the input file (must be a csv)" << std::endl;
        std::cout << "(-o, output.csv): name of the output file"
        std::cout << "(-s, 1e-4): epsilon parameter used to specify when program should terminate" << std::endl;
    }
    process_input(argc, argv, cfg);
    init_constants(cfg);
    feature, d_size = get_dimensions(input_file);
    std::vector<PDF<float, feature>> pdfs;
    std::vector<CDF<float, feature>> cdfs;
    std::vector<PPF<float, PPF_SIZE>> ppfs;
    pdfs.reserve(d_size);
    cdfs.reserve(d_size);
    ppfs.reserve(d_size);
    initialize_distributions(input_file, pdfs, cdfs, ppfs);
    WKmeans<Distribution<float, feature>> wkmeans(d_size, feature, epsilon, pdfs, cdfs);

    return 0;
}

