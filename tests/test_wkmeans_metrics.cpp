#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include "config.h"
#include "test_objects.h"
#include <catch2/catch_approx.hpp>
using namespace kmeans;
TEST_CASE("test_get_objective", "[metrics]"){
    PPF_SIZE = 500;
    build_data();
    double old_objective, f_objective;
    double new_objective = double{};
    WKmeans<double> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    f_objective = test_wkmeans.get_objective();
    test_wkmeans.update_clusters();
    test_wkmeans.update_bounds();
    test_wkmeans.swap_clusters();
    test_wkmeans.assign_new_clusters();
    //new_objective = test_wkmeans.get_objective();
    std::vector<int> cluster_assignments = test_wkmeans.get_cluster_assignments();
    new_objective = test_wkmeans.get_objective();

    int count = 0;
    for(int t = 0; t < 200; t++){
        old_objective = new_objective;
        test_wkmeans.update_clusters();
        test_wkmeans.update_bounds();
        std::vector<PPF<double>> old_clusters = test_wkmeans.clusters;
        std::vector<PPF<double>> new_clusters = test_wkmeans.get_new_clusters();
        test_wkmeans.swap_clusters();
        test_wkmeans.assign_new_clusters();
        new_objective = test_wkmeans.old_get_objective();
        cluster_assignments = test_wkmeans.get_cluster_assignments();
        //std::cout << test_wkmeans.delta_clusters(old_clusters, new_clusters) << " " << t << std::endl;
        if(new_objective > old_objective){
            std::cout << "old_objective: " <<  old_objective << " new objective: " << new_objective <<
            std::endl;
            count ++;
            }
        REQUIRE(count < 3);
    }
    std::cout << "first objective: " << f_objective << " last objective: " << new_objective<< std::endl;
}

TEST_CASE("test_working_threads", "[metrics]"){
    PPF_SIZE = 500;
    build_data();
    double new_objective;
    double test_objective{};
    WKmeans<double> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);

    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    for(int t = 0; t < 50; t++){
        test_wkmeans.update_clusters();
        test_wkmeans.update_bounds();
        test_wkmeans.swap_clusters();
        test_wkmeans.assign_new_clusters();
        new_objective = test_wkmeans.old_get_objective();
        std::vector<int> cluster_assignments = test_wkmeans.get_cluster_assignments();
        for(int i = 0; i < pdfs.size(); i ++){
            test_objective += wasserstein_2(ppfs[i], test_wkmeans.clusters[cluster_assignments[i]]);
        }
        REQUIRE(new_objective == Catch::Approx(test_objective).epsilon(1e-6));
        test_objective = double{};

}
}