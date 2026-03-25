#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include "containers/ppf.h"
#include "config.h"
#include "test_objects.h"
#include <limits>
#include <catch2/catch_approx.hpp>

using namespace kmeans;

double delta_clusters(std::vector<PPF<double>>& old_clusters,
                     std::vector<PPF<double>>& new_clusters)
{
    if (old_clusters.size() != new_clusters.size())
        throw std::runtime_error("Cluster vectors must be the same size");

    double sum = 0.0f;
    std::size_t k = old_clusters.size();

    for (std::size_t i = 0; i < k; i++) {
        sum += wasserstein_2(old_clusters[i], new_clusters[i]);
    }

    return sum / static_cast<double>(k);
}

TEST_CASE("test_update_clusters", "[clustering]"){
    build_data();
    WKmeans<double> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    test_wkmeans.update_clusters();
    std::vector<PPF<double>> new_clusters(N_CLUSTERS, PPF<double>(PPF_SIZE));
    std::vector<PPF<double>> test_new_clusters = test_wkmeans.get_new_clusters();
    for(int k = 0; k < N_CLUSTERS; k++){
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
    build_data();
    WKmeans<double> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    WKmeans<double> test_wkmeans2(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    test_wkmeans2.init_clusters();
    test_wkmeans2.init_bounds();
    REQUIRE(test_wkmeans.get_lower_bounds() == test_wkmeans2.get_lower_bounds());
    REQUIRE(test_wkmeans.get_upper_bounds() == test_wkmeans2.get_upper_bounds());
    REQUIRE(test_wkmeans.get_cluster_assignments() == test_wkmeans2.get_cluster_assignments());
    REQUIRE(test_wkmeans.get_cluster_size() ==test_wkmeans2.get_cluster_size());
    const auto& clusters1 = test_wkmeans.clusters;
    const auto& clusters2 = test_wkmeans2.clusters;
    REQUIRE(clusters1.size() == clusters2.size());
    for (size_t k = 0; k < clusters1.size(); k++) {
        REQUIRE(clusters1[k] == clusters2[k]); // uses your defined operator==
    }

REQUIRE(test_wkmeans2.clusters == test_wkmeans.clusters);
    std::vector<int> cluster_assignments(pdfs.size());
    for(int t = 0; t < 60; t++){
        test_wkmeans.update_clusters();
        test_wkmeans.update_bounds();
        test_wkmeans.swap_clusters();
        test_wkmeans2.update_clusters();
        test_wkmeans2.old_update_bounds();
        test_wkmeans2.swap_clusters();
        REQUIRE(test_wkmeans.clusters == test_wkmeans2.clusters);
        REQUIRE(test_wkmeans.get_cluster_assignments() == test_wkmeans2.get_cluster_assignments());
        REQUIRE(test_wkmeans.get_cluster_size()== test_wkmeans2.get_cluster_size());
        //REQUIRE(test_wkmeans2.get_lower_bounds() == test_wkmeans.get_lower_bounds());
        auto lb1 = test_wkmeans.get_lower_bounds();
        auto lb2 = test_wkmeans2.get_lower_bounds();
        for (size_t i = 0; i < lb1.size(); i++) {
            REQUIRE(lb1[i] == Catch::Approx(lb2[i]).epsilon(1e-5));
        }
        REQUIRE(test_wkmeans2.get_upper_bounds() == test_wkmeans.get_upper_bounds());

        std::vector<PPF<double>> clusters = test_wkmeans.clusters;
        test_wkmeans.assign_new_clusters();
        test_wkmeans2.assign_new_clusters();

        for(int i = 0; i < pdfs.size(); i++){
            double min_val = std::numeric_limits<double>::infinity();
            for(int k = 0; k < N_CLUSTERS; k++){
                double distance = wasserstein_2(ppfs[i], clusters[k]);
                if (distance < min_val){
                    min_val = distance;
                    cluster_assignments[i] = k;
                }
            }
        }
        REQUIRE(cluster_assignments == test_wkmeans.get_cluster_assignments());
        REQUIRE(test_wkmeans2.get_cluster_assignments() == test_wkmeans.get_cluster_assignments());
}

}