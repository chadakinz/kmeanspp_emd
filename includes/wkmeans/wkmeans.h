#pragma once
#include <vector>
#include "../config.h"
#include <random>

namespace kmeans {
    template<typename T, std::size_t N>
    class WKmeans{
    private:
        int n_clusters;
        int d_size;
        float epsilon;
        std::vector<PDF<T, N>> pdfs;
        std::vector<CDF<T, N>> cdfs;
        std::vector<PPF<T, PPF_SIZE>> ppfs;
        std::vector<float> upper_bounds;
        std::vector<float> lower_bounds;
        std::vector<PPF<T, PPF_SIZE>> clusters;
        std::mt19937 gen;

        void update_probability_vector(const NumericArray<T, M>& distances, const  std::vector<std::size_t>& updaed_distance
                ,const T& sum, NumericArray<T, M>& probs, const int& updated_distance_size);
    public:
        WKmeans(int data_size, int number_clusters, float eps, std::vector<PDF<T, N>>& prob_dense_funcs,
                         std::vector<CDF<T, N>>& cum_dense_funcs, std::vector<PPF<T, PPF_SIZE>>& percent_point_funcs);

        void init_clusters();
    };

}
#include "wkmeans_init_procedure.tpp"
#include "wkmeans_constructor.tpp"