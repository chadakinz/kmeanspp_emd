namespace kmeans{
    template<typename T>
    WKmeans<T>::WKmeans(int data_size, int number_clusters, float eps, std::vector<PDF<T>>& prob_dense_funcs,
            std::vector<CDF<T>>& cum_dense_funcs, std::vector<PPF<T>>& percent_point_funcs, int f, int seed)
            : epsilon(eps), d_size(data_size), n_clusters(number_clusters), pdfs(prob_dense_funcs), cdfs(cum_dense_funcs)
            ,ppfs(percent_point_funcs), features(f){
        clusters.reserve(n_clusters);
        std::random_device rd;
        if (seed == 0) gen.seed(rd());
        else gen.seed(seed);
    }
}