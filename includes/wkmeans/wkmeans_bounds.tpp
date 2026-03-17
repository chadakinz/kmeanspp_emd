#include <algorithm>
namespace kmeans{
    template <typename T>
    void WKmeans<T>::update_bounds(){
        for(int i = 0; i < d_size; i++){
            upper_bounds[i] += wasserstein_2(clusters[cluster_assignments[i]], new_clusters[cluster_assignments[i]]);
            r[i] = true;
            for(int k = 0; k < n_clusters; k++){
                int index = i * n_clusters + k;
                lower_bounds[index] = std::max(lower_bounds[index] - wasserstein_2(clusters[k], new_clusters[k]), T{});
            }
        }
    }

}