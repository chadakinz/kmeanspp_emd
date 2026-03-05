#pragma once
#include <string>

struct Config {
    int num_clusters = 10;
    float epsilon = 1e-4;
    std::string output_file = "output.csv";
    std::string input_file;

};

int NUM_CLUSTERS;
float EPSILON;
constexpr int PPF_SIZE = 20;
std::string INPUT_FILE;
std::string OUTPUT_FILE;

void init_constants(const Config& cfg) {
    NUM_CLUSTERS = cfg.num_clusters;
    EPSILON = cfg.epsilon;
    INPUT_FILE = cfg.input_file;
    OUTPUT_FILE = cfg.output_file;
}