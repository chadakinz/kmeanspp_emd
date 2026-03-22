#include <limits>
#include <thread>
#include <cstddef>
#include <algorithm>

namespace kmeans{
    template<typename T>
    void WKmeans<T>::update_probability_vector(const NumericArray<T>& distances
            ,const T& sum, NumericArray<T>& probs){
        int j;
        for(int i = 0; i < distances.size(); i++){
            probs[i] = distances[i]/sum;
        }
    }
    //function gets a list of clusters, where each cluster is going to be a ppf
    template<typename T>
    void WKmeans<T>::init_clusters(){
        T sum{};
        std::fill(cluster_size.begin(), cluster_size.end(), 0);
        std::size_t M = pdfs.size();
        //NumericArray<T> probs(M);
        NumericArray<T> distances(M, std::numeric_limits<T>::infinity());
        std::uniform_int_distribution<int> dist(0, M-1);

        //randomly select the first cluster
        int random_cluster_index = dist(gen);
        PPF<T> current_cluster = ppfs[random_cluster_index];
        clusters[0] = current_cluster;

        int num_threads = std::thread::hardware_concurrency();
        int chunk = (M + num_threads - 1)/num_threads;

        //iterate through and add clusters 2-n_clusters
        for(std::size_t k = 1; k < n_clusters; k++){
            std::vector<std::thread> threads;
            for(int t = 0; t < num_threads; t++){
                int start = t * chunk;
                int end = std::min(start + chunk, (int)M);
                threads.emplace_back([=, this, &distances, &current_cluster](){
                    for(std::size_t i = start; i < end; i++) {
                        T x = wasserstein_2(ppfs[i], current_cluster);
                        T y = x * x;
                        //if the new distance is greater than the previous distance, update the sum, add the updated index to a list
                        if (distances[i] > y) {
                            distances[i] = y;
                        }
                    }
                });


            }
            for (auto& th : threads) th.join();
            //update the probability vector of our data
            //update_probability_vector(distances, sum, probs);
            std::discrete_distribution<> dist(distances.begin(), distances.end());
            int sample = dist(gen);

            //assign the new cluster to whatever data point we randomly selected last
            current_cluster = ppfs[sample];
            // add cluster to list
            clusters[k] = current_cluster;
        }

    }

    template <typename T>
    void WKmeans<T>::init_bounds(){
        int num_threads = std::thread::hardware_concurrency();
        int chunk = (d_size + num_threads - 1)/num_threads;
        std::vector<std::vector<int>> local_counts(
                num_threads, std::vector<int>(n_clusters, 0)
        );
        std::vector<std::thread> threads;
        for(int t = 0; t < num_threads; t++){
            int start = t *chunk;
            int end = std::min(chunk + start, d_size);
            threads.emplace_back([=, this, &local_counts](){
                for(int i = start; i < end; i++){
                    int min_cluster_index;
                    T min = std::numeric_limits<T>::max();
                    for(int k = 0; k < n_clusters; k++){
                        T distance = wasserstein_2(clusters[k], ppfs[i]);
                        if(distance < min){
                            min = distance;
                            min_cluster_index = k;
                        }
                        lower_bounds[i*n_clusters + k] = distance;
                    }
                    upper_bounds[i] = min;
                    (*this).cluster_assignments[i] = min_cluster_index;
                    local_counts[t][min_cluster_index] += 1;
                }
            });
        }
        for (auto& th : threads) th.join();

        for(int t = 0; t<num_threads;t++){
            for(int k = 0; k < n_clusters; k++){
                (*this).cluster_size[k] += local_counts[t][k];
            }
        }


    }
}