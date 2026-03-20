#include <iostream>
namespace kmeans{
    template <typename T>
    T WKmeans<T>::run_restart(){
        init_clusters();
        init_bounds();
        bool converged = false;
        while (!converged){
            update_clusters();
            update_bounds();
            if(delta_clusters(new_clusters, clusters) < epsilon) converged = true;
            std::cout << delta_clusters(new_clusters, clusters) << " delta clusters"<< std::endl;
            swap_clusters();
            assign_new_clusters();
        }
        return get_objective();
    }
}
