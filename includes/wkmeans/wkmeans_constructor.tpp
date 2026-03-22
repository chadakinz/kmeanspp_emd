#include <iostream>
namespace kmeans{
    template<typename T>
    WKmeans<T>::WKmeans(int data_size, int number_clusters, float eps, const std::vector<PDF<T>>& prob_dense_funcs,
            const std::vector<CDF<T>>& cum_dense_funcs, const std::vector<PPF<T>>& percent_point_funcs, int f, int seed)
            : epsilon(eps), d_size(prob_dense_funcs.size()), n_clusters(number_clusters), pdfs(prob_dense_funcs), cdfs(cum_dense_funcs)
            ,ppfs(percent_point_funcs), features(f){

        clusters.resize(n_clusters);
        upper_bounds.resize(d_size);
        lower_bounds.resize(d_size*n_clusters);
        cluster_assignments.resize(d_size);
        cluster_size.resize(n_clusters);
        new_clusters.resize(n_clusters);
        std::random_device rd;
        r.resize(d_size, true);
        if (seed == 0) gen.seed(rd());
        else gen.seed(seed);
    }

}