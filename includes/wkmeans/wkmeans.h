#pragma once
#include <vector>
#include "../config.h"
#include <random>
#include "containers/pdf.h"
#include "containers/cdf.h"
#include "containers/ppf.h"

namespace kmeans {
    template<typename T>
    class WKmeans{
    private:
        int n_clusters, d_size, features;
        float epsilon;
        std::vector<PDF<T>> pdfs;
        std::vector<CDF<T>> cdfs;
        std::vector<PPF<T>> ppfs;
        std::vector<T> upper_bounds;
        std::vector<T> lower_bounds;
        std::mt19937 gen;

        void update_probability_vector(const NumericArray<T>& distances, const  std::vector<std::size_t>& updated_distance
                ,const T& sum, NumericArray<T>& probs, const int& updated_distance_size);
    public:
        std::vector<PPF<T>> clusters;
        WKmeans(int data_size, int number_clusters, float eps, std::vector<PDF<T>>& prob_dense_funcs,
                         std::vector<CDF<T>>& cum_dense_funcs, std::vector<PPF<T>>& percent_point_funcs, int f, int seed = 0);

        void init_clusters();
    };

}
#include "wkmeans_init_procedure.tpp"
#include "wkmeans_constructor.tpp"