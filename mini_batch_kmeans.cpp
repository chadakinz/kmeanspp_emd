#include <filesystem>
#include <vector>
#include <iostream>
#include "wkmeans/wkmeans.h"
#include <utility>
#include <array>
#include "containers/cdf.h"
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "distributions.hpp"
#include "config.h"
#include <thread>
#include <atomic>
#include <limits>
#include <cstring>

//input: folder of samples, all should be the same size
//output: cluster assignments following the batch kmeans algorithm
//initalize clusters to begin, standard procedure
//input needs to be directory and file name. Going to assume file names are in the format director/filename_i where i is going to be each sample
//files must begin at 0, where 0 is going to be the intialization of clusters
using namespace kmeans;

std::string build_path(const std::string& dir, const std::string& filename) {
    fs::path full_path = fs::path(dir) / filename;
    return full_path.string();  // convert to std::string
}

int number_of_files(const std::string& directory, const std::string& file_name){
    int file_size = file_name.size();
    int count = 0;
    std::string prefix = file_name + "_";
    for(auto& p: fs::directory_iterator(directory)){
        std::string filename = p.path().filename().string();

        if(!fs::is_regular_file(p)) continue;
        if(filename.size() < file_size + 1) continue;
        if(filename.substr(0, file_size + 1) == prefix){
            count ++;
        }
    }
    return count;
}
void process_input(int argc, char* argv[], Config& cfg){
    for(int i = 1; i < argc; i++){  // start at 1 (skip program name)

        if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            cfg.num_clusters = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc){
            cfg.epsilon = std::stof(argv[++i]);
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc){
            cfg.output_file = argv[++i];
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc){
            cfg.input_file = argv[++i];
        }
        else if (strcmp(argv[i], "-u") == 0 && i + 1 < argc){
            cfg.ppf_size = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc){
            cfg.n_threads = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-N") == 0 && i + 1 < argc){
            cfg.n_restarts = std::stoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-S") == 0 && i + 1 < argc){
            cfg.seed = std::stoi(argv[++i]);
        }
        else if(strcmp(argv[i], "-d") == 0 && i + 1 < argc){
            cfg.directory = argv[++i];
        }
        else {
            std::cerr << "Unknown or incomplete argument: " << argv[i] << std::endl;
        }
    }
}
std::pair<int,int> get_dimensions(const std::string& file_name) {
    std::ifstream file(file_name);

    std::string first_line;
    std::getline(file, first_line);
    std::cout << first_line << " first line" << std::endl;
    int columns = std::count(first_line.begin(), first_line.end(), ' ') + 1;
    std::cout << columns << " columns" << std::endl;
    int rows = 1 + std::count(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>(),
            '\n'
    );

    return {rows, columns};
}

int main(int argc, char* argv[]) {
    Config cfg;
    std::string output_file = "output_999.csv";

    if (argc == 1) {
        std::cout << "(flag, default): description" << std::endl;
        std::cout << "(-k, 10): number of clusters" << std::endl;
        std::cout << "(-i, NONE): name of the input file (must be a csv)" << std::endl;
        std::cout << "(-d, \"\"): name of the directory" << std::endl;
        std::cout << "(-o, output_999.csv): name of the output file" << std::endl;
        std::cout << "(-s, 1e-4): epsilon parameter used to specify when program should terminate" << std::endl;
        std::cout << "(-u, 20): size of percent point function bins" << std::endl;
        std::cout << "(-N, 50): Number of restarts to run algorithm" << std::endl;
        std::cout << "(-t, 1): number of threads to run the algorithm (cannot exceed Number of restarts)" << std::endl;
        std::cout << "(S, 0): choose the specify the seed for the random number generator" << std::endl;
    }
    process_input(argc, argv, cfg);
    init_constants(cfg);

    std::string init_file = build_path(DIRECTORY, INPUT_FILE + "_0");
    std::pair<int, int> pair = get_dimensions(init_file);
    int d_size = pair.first;
    int feature = pair.second;

    std::vector<PDF<double>> pdfs;
    std::vector<CDF<double>> cdfs;
    std::vector<PPF<double>> ppfs;
    pdfs.reserve(d_size);
    cdfs.reserve(d_size);
    ppfs.reserve(d_size);

    int number_samples = number_of_files(DIRECTORY, INPUT_FILE);

    init_distributions(init_file, pdfs, cdfs, ppfs);

    //initialize the first clusters using the 0 sample
    WKmeans<double> wkmeans_obj(pdfs.size(), NUM_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, feature, SEED);
    double objective = wkmeans_obj.run_restart();

    for(int i = 1; i < number_samples; i++){
        std::vector<PPF<double>> previous_clusters = wkmeans_obj.clusters;
        std::string format_file = "_" + std::stoi(i);
        std::string init_file = build_path(DIRECTORY, format_file);
        init_distributions(init_file, pdfs, cdfs, ppfs);
        //todo implement a way to reset pdfs, cdfs, ppfs from an already created object.
        wkmeans_obj.reset_for_next_batch();
        wkmeans_obj.init_batch_kmeans(pdfs, cdfs, ppfs, previous_clusters);
    }
}