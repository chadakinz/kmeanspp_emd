namespace kmeans{
    template <typename T>
    T WKmeans<T>::delta_clusters(std::vector<PPF<T>>& old_clusters,
                         std::vector<PPF<T>>& new_clusters)
    {
        T sum = T{};
        std::size_t k = old_clusters.size();

        for (std::size_t i = 0; i < k; i++) {
            sum += wasserstein_2(old_clusters[i], new_clusters[i]);
        }

        return sum / static_cast<T>(k);
    }
    //Function that gets the objective of the kmeans clustering algorithm
    template <typename T>
    T WKmeans<T>::get_objective(){
        T sum{};
        for(int i = 0; i < d_size; i++){
            for(int k = 0; k < n_clusters; k++){
                sum += wasserstein_2(ppfs[i], clusters[cluster_assignments[i]]);
            }

        }
        return sum;
    }
}

