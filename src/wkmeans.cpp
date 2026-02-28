#include "../includes/wkmeans.h"
namespace kmeans {
    template<typename T>
    WKmeans::WKmeans(int data_size, int number_clusters, float eps, std::vector<T> prob_dense_funcs, std::vector<T> cum_dense_funcs)
    : epsilon(eps), d_size(data_size), n_clusters(number_clusters), pdfs(prob_dense_funcs), cdfs(cum_dense_funcs)
}
