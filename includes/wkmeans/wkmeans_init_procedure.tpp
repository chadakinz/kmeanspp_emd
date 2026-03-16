#include <limits>

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
        T x, y;
        std::size_t M = pdfs.size();
        NumericArray<T> probs(M);
        NumericArray<T> distances(M);
        for(std::size_t i = 0; i < M; i++)
            distances[i] = std::numeric_limits<T>::infinity();
        std::uniform_int_distribution<int> dist(0, M-1);

        //randomly select the first cluster
        int random_cluster_index = dist(gen);
        PPF<T> current_cluster = ppfs[random_cluster_index];
        clusters.push_back(current_cluster);

        //iterate through and add clusters 2-n_clusters
        for(std::size_t k = 1; k < n_clusters; k++){
            for(std::size_t i = 0; i < M; i++){
                x = wasserstein_2(ppfs[i], current_cluster);
                y = x * x;
                //if the new distance is greater than the previous distance, update the sum, add the updated index to a list
                if (distances[i] > y){
                    if (!std::isinf(distances[i]))
                        sum -= distances[i];
                    distances[i] = y;
                    sum += y;
                }
            }
            //update the probability vector of our data
            update_probability_vector(distances, sum, probs);
            std::discrete_distribution<> dist(probs.begin(), probs.end());
            int sample = dist(gen);

            //assign the new cluster to whatever data point we randomly selected last
            current_cluster = ppfs[sample];
            // add cluster to list
            clusters.push_back(current_cluster);
        }
    }

    template <typename T>
    void WKmeans<T>::init_bounds(){
        T min = std::numeric_limits<T>::max();
        T distance;
        int min_cluster_index;
        for(int i = 0; i < d_size; i++){
            min = std::numeric_limits<T>::max();
            for(int k = 0; k < n_clusters; k++){
                distance = wasserstein_2(clusters[k], ppfs[i]);
                if(distance < min){
                    min = distance;
                    min_cluster_index = k;
                }
                lower_bounds[i*n_clusters + k] = distance;
            }
            upper_bounds[i] = min;
            (*this).cluster_assignments[i] = min_cluster_index;
            (*this).cluster_size[min_cluster_index] += 1;
        }
    }
}