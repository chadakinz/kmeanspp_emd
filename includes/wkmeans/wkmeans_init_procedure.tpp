namespace kmeans{
    template<typename T>
    void WKmeans<T>::update_probability_vector(const NumericArray<T>& distances, const  std::vector<std::size_t>& updated_distance
            ,const T& sum, NumericArray<T>& probs, const int& updated_distance_size){
        for(int i = 0; i < updated_distance_size; i++){
            j = updated_distance[i];
            probs[j] = distances[j]/sum;
        }
    }
    template<typename T>
    void WKmeans<T>::init_clusters(){
        T sum{};
        T x, y;
        std::size_t M = pdfs.size();
        NumericArray<T> probs(d_size);
        NumericArray<T> distances(d_size);
        std::uniform_int_distribution<int> dist(0, M-1);
        int random_cluster_index = dist(gen);
        PPF<T, PPF_SIZE> current_cluster = ppfs[random_cluster_index];
        clusters.push_back(current_cluster);
        int updated_distance_size = 0;
        std::vector<std::size_t> updated_distance;
        updated_distance.resize(M);
        for(std::size_t k = 1; k < n_clusters; k++){
            for(std::size_t i = 0; i < M; i++){
                x = wasserstein_2(ppfs[i], current_cluster);
                y = x * x;
                if (distances[i] > y){
                    sum -= distances[i] - y;
                    distances[i] = y;
                    updated_distance[updated_distance_size ++] = i;
                }else if(distances[i] == 0){
                    distances[i] = y;
                    sum += y;
                    updated_distance[updated_distance_size ++] = i;
                }
            }
            update_probability_vector(distances, updated_distance, sum, probs, updated_distance_size);
            std::discrete_distribution<> dist(probs.begin(), probs.end());
            int sample = dist(gen);
            new_cluster = ppfs[sample];
            updated_distance_size = 0;
            clusters.push_back(new_cluster);

        }
    }
}