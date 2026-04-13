#include <iostream>
#include <chrono>
namespace kmeans{
    template <typename T>
    T WKmeans<T>::run_restart(){
        auto now = std::chrono::high_resolution_clock::now();

        init_clusters();
        init_bounds();

        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end - now).count();
        std::cout << "Init procedure took " << elapsed << " seconds\n";
        bool converged = false;

        while (!converged){
            auto now = std::chrono::high_resolution_clock::now();
            update_clusters();
            update_bounds();
            if(delta_clusters(new_clusters, clusters) < epsilon) converged = true;
            std::cout << delta_clusters(new_clusters, clusters) << " delta clusters"<< std::endl;
            swap_clusters();
            assign_new_clusters();

            auto end = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(end - now).count();
            std::cout << "Update procedure took " << elapsed << " seconds\n";
        }
        return get_objective();
    }
    //TODO dont need any upper or lower bounds, just need to assign clusters for one iteration
    template<typename T>
    void WKmeans<T>::run_batch_restart(const std::vector<uint32_t>& sizes){
      mini_batch_assign_clusters();
      mini_batch_update_clusters(sizes);
      }

}
