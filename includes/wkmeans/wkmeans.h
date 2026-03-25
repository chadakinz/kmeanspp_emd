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
        std::vector<int> cluster_assignments;
        std::vector<int> cluster_size;
        std::mt19937 gen;
        std::vector<PPF<T>> new_clusters;
        std::vector<bool> r;

        void update_probability_vector(const NumericArray<T>& distances
                ,const T& sum, NumericArray<T>& probs);

        inline std::vector<T> get_vector_s();
        T delta_clusters(std::vector<PPF<T>>& old_clusters, std::vector<PPF<T>>& new_clusters);



    public:
        std::vector<PPF<T>> clusters;
        WKmeans(int data_size, int number_clusters, float eps, const std::vector<PDF<T>>& prob_dense_funcs,
                         const std::vector<CDF<T>>& cum_dense_funcs, const std::vector<PPF<T>>& percent_point_funcs, int f, int seed = 0);

        void init_clusters();
        void init_bounds();
        inline void swap_clusters();
        std::vector<PDF<T>>convert_clusters();
        void update_bounds();
        void update_clusters();
        void assign_new_clusters();
        T get_objective();
        T run_restart();
        void old_update_bounds();
        T old_get_objective();

        const std::vector<T>& get_upper_bounds() const { return upper_bounds; }
        const std::vector<T>& get_lower_bounds() const { return lower_bounds; }
        const std::vector<int>& get_cluster_assignments() const { return cluster_assignments; }
        const std::vector<int>& get_cluster_size() const { return cluster_size; }
        const std::vector<PPF<T>>& get_new_clusters() const { return new_clusters; }
    };
}
#include "wkmeans_init_procedure.tpp"
#include "wkmeans_constructor.tpp"
#include "wkmeans_clustering.tpp"
#include "wkmeans_bounds.tpp"
#include "wkmeans_metrics.tpp"
#include "wkmeans_run_procedure.tpp"
