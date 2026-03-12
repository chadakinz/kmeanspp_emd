namespace kmeans{
    template<typename T>
    void WKmeans<T>::update_probability_vector(const NumericArray<T>& distances, const  std::vector<std::size_t>& updated_distance
            ,const T& sum, NumericArray<T>& probs, const int& updated_distance_size){
        int j;
        for(int i = 0; i < updated_distance_size; i++){
            j = updated_distance[i];
            probs[j] = distances[j]/sum;
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
        std::uniform_int_distribution<int> dist(0, M-1);

        //randomly select the first cluster
        int random_cluster_index = dist(gen);
        PPF<T> current_cluster = ppfs[random_cluster_index];
        clusters.push_back(current_cluster);

        //updated_distances_size tracks how many points in our dataset we have found a closer cluster to
        int updated_distance_size = 0;
        std::vector<std::size_t> updated_distance;
        updated_distance.resize(M);

        //iterate through and add clusters 2-n_clusters
        for(std::size_t k = 1; k < n_clusters; k++){
            for(std::size_t i = 0; i < M; i++){
                x = wasserstein_2(ppfs[i], current_cluster);
                y = x * x;
                //if the new distance is greated than the previous distance, update the sum, add the updated index to a list
                if (distances[i] > y){
                    sum -= distances[i] - y;
                    distances[i] = y;
                    updated_distance[updated_distance_size ++] = i;
                }
                //if the current distance of this point is 0, then update it
                else if(distances[i] == 0){
                    distances[i] = y;
                    sum += y;
                    updated_distance[updated_distance_size ++] = i;
                }
            }
            //update the probability vector of our data using the list of updated distances
            update_probability_vector(distances, updated_distance, sum, probs, updated_distance_size);
            std::discrete_distribution<> dist(probs.begin(), probs.end());
            int sample = dist(gen);

            //assign the new cluster to whatever data point we randomly selected last
            current_cluster = ppfs[sample];
            updated_distance_size = 0;

            // add cluster to list
            clusters.push_back(current_cluster);
        }
    }
}