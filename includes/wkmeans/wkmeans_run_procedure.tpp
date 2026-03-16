namespace kmeans{
    template <typename T>
    void WKmeans<T>::run_once(){
        init_clusters();
        init_bounds();
        while (!converged){
            update_clusters();
            update_bounds();

            assign_new_clusters();
            get_objective();
        }
    }
}
