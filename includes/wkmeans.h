#pragma once
#include <vector>
namespace kmeans {
    template<typename T>

    class WKmeans:
    public:
        WKmeans(int data_size, int number_clusters, float eps, std::vector<T> prob_dense_funcs, std::vector<T> cum_dense_funcs);

    private:
        int n_clusters;
        int d_size;
        float epsilon;
        std::vector<T> pdfs;
        std::vector<T> cdfs;
        std::vector<float> upper_bounds;
        std::vector<float> lower_bounds;
        std::vector<T> clusters;

}