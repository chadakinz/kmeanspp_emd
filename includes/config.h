#pragma once
#include <string>

struct Config {
    int num_clusters = 10;
    float epsilon = 1e-4;
    std::string output_file = "output.csv";
    std::string input_file;
    int ppf_size = 50;
    int n_threads = 1;
    int n_restarts = 50;
    int seed = 0;

};

int NUM_CLUSTERS;
float EPSILON;
int PPF_SIZE = 500;
int N_RESTARTS = 50;
int N_THREADS = 1;
int SEED = 0;
std::string INPUT_FILE;
std::string OUTPUT_FILE;

void init_constants(const Config& cfg) {
    NUM_CLUSTERS = cfg.num_clusters;
    EPSILON = cfg.epsilon;
    INPUT_FILE = cfg.input_file;
    OUTPUT_FILE = cfg.output_file;
    PPF_SIZE = cfg.ppf_size;
    N_RESTARTS = cfg.n_restarts;
    N_THREADS = cfg.n_threads;
    SEED = cfg.seed;
}