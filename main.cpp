#include <vector>
#include <iostream>
#include "wkmeans/wkmeans.h"
#include <utility>
#include <algorithm>
#include <array>
#include "containers/cdf.h"
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "distributions.hpp"
#include "config.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <limits>
#include <cstring>
#include <functional>
using namespace kmeans;
std::mutex mtx;
std::atomic<float> global_min(std::numeric_limits<float>::infinity());

std::atomic<int> counter(0);

void write_clusters_to_output(const std::vector<PDF<float>>& clusters){
    std::ofstream out(OUTPUT_FILE);
    if (!out.is_open()) {
        std::cerr << "Error: cannot open file " << OUTPUT_FILE << "\n";
        return;
    }
    for (const auto& pdf : clusters) {
        for (size_t i = 0; i < pdf.size(); ++i) {
            out << pdf[i];
            if (i + 1 < pdf.size()) out << ",";  // comma between elements
        }
        out << "\n";  // new row per PDF
    }
    out.close();

}

void worker(const std::vector<PDF<float>>& pdfs, const std::vector<CDF<float>>& cdfs, const std::vector<PPF<float>>& ppfs,
            int features, int id, int seed = 0){
    std::cout << pdfs.size() << " pdfs.size()" << std::endl;
    printf("INSIDE THREAD\n");
    float objective;
    if (id == 0){printf("INSIDE THREAD\n");}
    WKmeans<float> wkmeans_obj(pdfs.size(), NUM_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, features, seed);
    printf("OBJECT CREATED\n");
    if (id == 0){printf("OBJECT CREATED");}
    while (true){
        int old = counter.fetch_add(1);
        printf("COUNTER FETCH ADD?\n");

        if(old >= N_RESTARTS) break;
        objective = wkmeans_obj.run_restart();
        std::cout << "RAN RESTART OBJECTIVE RECEIVED: " << objective << std::endl;
        if (objective >= global_min) continue;
        std::lock_guard<std::mutex> lock(mtx);
        if (objective < global_min.load()){
            std::cout << "NEW GLOBAL MIN OBJECTIVE FOUND AFTER" << counter << " RESTARTS" << std::endl;
            std::cout << "Old objective: " << global_min.load() << " New Objective: " << objective << std::endl;
            global_min = objective;
            write_clusters_to_output(wkmeans_obj.convert_clusters());
        }
    }
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

int main(int argc, char* argv[]){
    Config cfg;
    int feature, d_size;
    std::string output_file = "output.csv";

    //TODO: need to pass an argument specifying how many restarts the user wants to run the algorithm with
    if (argc == 1){
        std::cout << "(flag, default): description" << std::endl;
        std::cout << "(-k, 10): number of clusters" << std::endl;
        std::cout << "(-i, NONE): name of the input file (must be a csv)" << std::endl;
        std::cout << "(-o, output.csv): name of the output file" << std::endl;
        std::cout << "(-s, 1e-4): epsilon parameter used to specify when program should terminate" << std::endl;
        std::cout << "(-u, 20): size of percent point function bins" << std::endl;
        std::cout << "(-N, 50): Number of restarts to run algorithm" << std::endl;
        std::cout << "(-t, 1): number of threads to run the algorithm (cannot exceed Number of restarts)" << std::endl;
        std::cout << "(S, 0): choose the specify the seed for the random number generator" << std::endl;
    }
    //processing inputs
    //TODO: normallize ppfs before passing them through function
    process_input(argc, argv, cfg);
    printf("INPUTS PROCESSED\n");
    init_constants(cfg);
    std::pair<int, int> pair = get_dimensions(INPUT_FILE);
    printf("GOT DIMS\n");

    d_size = pair.first;
    feature = pair.second;
    std::vector<PDF<float>> pdfs;
    std::vector<CDF<float>> cdfs;
    std::vector<PPF<float>> ppfs;
    pdfs.reserve(d_size);
    cdfs.reserve(d_size);
    ppfs.reserve(d_size);
    std::cout << d_size << " d_size" << std::endl;
    printf("RESERVED D_SIZE\n");
    init_distributions(INPUT_FILE, pdfs, cdfs, ppfs, feature);
    printf("INIT DISTS\n");
    pdfs[5].print();
    std::vector<std::thread> threads;
    threads.reserve(N_THREADS);
    std::cout << N_THREADS << " N_THREADS <-----" << std::endl;
    for(int i = 0; i < N_THREADS; i++){
        threads.emplace_back(worker, std::cref(pdfs), std::cref(cdfs), std::cref(ppfs), feature, i, SEED);
    }
    printf("FINISHED BUILDING THREADS\n");
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}

