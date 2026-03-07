namespace kmeans{
    template<typename T, std::size_t N>
    WKmeans<T, N>::WKmeans(int data_size, int number_clusters, float eps, std::vector<PDF<T, N>>& prob_dense_funcs,
            std::vector<CDF<T, N>>& cum_dense_funcs, std::vector<PPF<T, PPF_SIZE>>& percent_point_funcs )
            : epsilon(eps), d_size(data_size), n_clusters(number_clusters), pdfs(prob_dense_funcs), cdfs(cum_dense_funcs)
            ,ppfs(percent_point_funcs) {
        clusters.reserve(n_clusters);
        std::random_device rd;
        gen.seed(rd());
    }
}