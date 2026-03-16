#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include <iostream>
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "containers/cdf.h"
#include "config.h"
#include "test_objects.h"
#include <limits>


using namespace kmeans;

float delta_clusters(std::vector<PPF<float>>& old_clusters,
                     std::vector<PPF<float>>& new_clusters)
{
    if (old_clusters.size() != new_clusters.size())
        throw std::runtime_error("Cluster vectors must be the same size");

    float sum = 0.0f;
    std::size_t k = old_clusters.size();

    for (std::size_t i = 0; i < k; i++) {
        sum += wasserstein_2(old_clusters[i], new_clusters[i]);
    }

    return sum / static_cast<float>(k);
}

TEST_CASE("test_update_clusters", "[clustering]"){
    PPF_SIZE = 20;
    build_data();
    WKmeans<float> test_wkmeans(pdfs.size(), 4, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    test_wkmeans.update_clusters();
    std::vector<PPF<float>> new_clusters(4, PPF<float>(PPF_SIZE));
    std::vector<PPF<float>> test_new_clusters = test_wkmeans.get_new_clusters();
    for(int k = 0; k < 4; k++){
        new_clusters[k].zero();
    }

    for(int i = 0; i < pdfs.size(); i++){
        new_clusters[test_wkmeans.get_cluster_assignments()[i]] += ppfs[i];
    }
    for(int k = 0; k < N_CLUSTERS; k++){
        new_clusters[k] /= test_wkmeans.get_cluster_size()[k];
    }
    for (int k = 0; k < N_CLUSTERS; k++){
        REQUIRE(new_clusters[k] == test_new_clusters[k]);
    }
}
//Assign new clusters needs to test if clusters have been reassigned properly without doing any bounds tricks. And then needs to check to see if the lower and upper bounds are updated properly
TEST_CASE("test_assign_new_clusters", "[clustering]"){
    PPF_SIZE = 20;
    build_data();
    WKmeans<float> test_wkmeans(pdfs.size(), 4, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    std::vector<int> cluster_assignments(pdfs.size());
    for(int t = 0; t < 10; t++){
        test_wkmeans.update_clusters();
        test_wkmeans.update_bounds();
        std::vector<PPF<float>> old_clusters = test_wkmeans.clusters;
        test_wkmeans.swap_clusters();
        std::vector<PPF<float>> clusters = test_wkmeans.clusters;
        float min_val = std::numeric_limits<float>::infinity();
        for(int i = 0; i < pdfs.size(); i++){
            min_val = std::numeric_limits<float>::infinity();
            for(int k = 0; k < 4; k++){
                float distance = wasserstein_2(ppfs[i], clusters[k]);
                if (distance < min_val){
                    min_val = distance;
                    cluster_assignments[i] = k;
                }
            }
        }
        test_wkmeans.assign_new_clusters();
        std::cout << "delta clusters for iter " << t << ": " << delta_clusters(old_clusters, clusters) << "\n";
        REQUIRE(cluster_assignments == test_wkmeans.get_cluster_assignments());
}

}