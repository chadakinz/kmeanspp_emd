#include <catch2/catch_test_macros.hpp>
#include "containers/ppf.h"
#include "containers/pdf.h"
#include "containers/cdf.h"
#include "wkmeans/wkmeans.h"
#include "test_objects.h"
#include <iostream>
#include <algorithm>

using namespace kmeans;
//test case is going to be a bit redundant
TEST_CASE("test_update_bounds", "[bounds]"){
    PPF_SIZE = 20;
    build_data();
    WKmeans<double> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    test_wkmeans.update_clusters();
    std::vector<double> upper_bounds = test_wkmeans.get_upper_bounds();
    std::vector<double> lower_bounds = test_wkmeans.get_lower_bounds();
    std::vector<int> cluster_assignments = test_wkmeans.get_cluster_assignments();
    std::vector<PPF<double>> clusters = test_wkmeans.clusters;
    std::vector<PPF<double>> new_clusters = test_wkmeans.get_new_clusters();
    test_wkmeans.update_bounds();
    for(int i = 0; i < pdfs.size(); i ++){
        upper_bounds[i] += wasserstein_2(clusters[cluster_assignments[i]], new_clusters[cluster_assignments[i]]);
    }
    REQUIRE(upper_bounds == test_wkmeans.get_upper_bounds());

    for(int i = 0; i < pdfs.size(); i++){
        for(int k = 0; k < N_CLUSTERS; k++){
            double distance = wasserstein_2(clusters[k], new_clusters[k]);
            lower_bounds[i*N_CLUSTERS + k] = std::max(lower_bounds[i*N_CLUSTERS + k] - distance, double{});
        }
    }
    REQUIRE(lower_bounds == test_wkmeans.get_lower_bounds());
}