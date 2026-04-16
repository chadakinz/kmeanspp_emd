#include <thread>
#include <algorithm>
namespace kmeans{
    template <typename T>
    T WKmeans<T>::delta_clusters(std::vector<PPF<T>>& old_clusters,
                         std::vector<PPF<T>>& new_clusters)
    {
        T sum = T{};
        for (std::size_t i = 0; i < n_clusters; i++) {
            sum += wasserstein_2(old_clusters[i], new_clusters[i]);
        }
        return sum / static_cast<T>(n_clusters);
    }
    //Function that gets the objective of the kmeans clustering algorithm
    template <typename T>
    T WKmeans<T>::get_objective(){
        T sum{};
        int num_threads = std::thread::hardware_concurrency();
        int chunk = (d_size + num_threads - 1)/num_threads;
        std::vector<std::thread> threads;
        std::vector<T> thread_local_sum(num_threads, T{});
        for(int t = 0; t < num_threads; t++){
            int start = t * chunk;
            int end = std::min(start + chunk, d_size);
            threads.emplace_back([=, this, &thread_local_sum](){
                for(int i = start; i < end; i++){
                    thread_local_sum[t] += wasserstein_2(ppfs[i], clusters[cluster_assignments[i]]);
                }
            });
        }
        for(int t = 0; t < num_threads; t++){
            threads[t].join();
        }
        for(int t = 0; t < num_threads; t++){
            sum += thread_local_sum[t];
        }
        return sum;
    }
    template <typename T>
    T WKmeans<T>::old_get_objective(){
        T sum{};
        for(int i = 0; i < d_size; i++){
            sum += wasserstein_2(ppfs[i], clusters[cluster_assignments[i]]);
        }
        return sum;
    }
}

