#pragma once
#include <vector>
#include "config.h"
#include <random>

namespace kmeans {
    template<typename T, std::size_t N>
    class WKmeans{
    private:
        int n_clusters;
        int d_size;
        float epsilon;
        std::vector<PDF<T, N>> pdfs;
        std::vector<CDF<T, N>> cdfs;
        std::vector<PPF<T, PPF_SIZE>> ppfs;
        std::vector<float> upper_bounds;
        std::vector<float> lower_bounds;
        std::vector<PPF<T, PPF_SIZE>> clusters;
        std::mt19937 gen;
        
        template<int M>
        void update_probability_vector(const NumericArray<T, M>& distances, const  std::vector<std::size_t>& updaed_distance
        ,const T& sum, NumericArray<T, M>& probs, const int& updated_distance_size){
            for(int i = 0; i < updated_distance_size; i++){
                j = updated_distance[i];
                probs[j] = distances[j]/sum;
            }

        }
    public:
        WKmeans(int data_size, int number_clusters, float eps, std::vector<PDF<T, N>>& prob_dense_funcs,
                         std::vector<CDF<T, N>>& cum_dense_funcs, std::vector<PPF<T, PPF_SIZE>>& percent_point_funcs )
                : epsilon(eps), d_size(data_size), n_clusters(number_clusters), pdfs(prob_dense_funcs), cdfs(cum_dense_funcs)
                ,ppfs(percent_point_funcs) {
            clusters.reserve(n_clusters);
            std::random_device rd;
            gen.seed(rd());
        }

        void init_clusters(){
            T sum{};
            T x, y;
            std::size_t M = pdfs.size();
            NumericArray<T, M> probs;
            NumericArray<T, M> distances{};
            std::uniform_int_distribution<int> dist(0, M-1);
            int random_cluster_index = dist(gen);
            PPF<T, PPF_SIZE> current_cluster = ppfs[random_cluster_index];
            clusters.push_back(current_cluster);
            int updaed_distance_size = 0;
            std::vector<std::size_t> updaed_distance;
            updaed_distance.resize(M);
            for(std::size_t k = 1; k < n_clusters; k++){
                for(std::size_t i = 0; i < M; i++){
                    x = wasserstein_2(ppfs[i], current_cluster);
                    y = x * x;
                    if (distances[i] > y){
                        sum -= distances[i] - y;
                        distances[i] = y;
                        updaed_distance[updaed_distance_size ++] = i;
                    }else if(distances[i] == 0){
                        distances[i] = y;
                        sum += y;
                        updaed_distance[updaed_distance_size ++] = i;
                    }
                }
                update_probability_vector();
            }
        }
    };

}