#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include "containers/ppf.h"
#include "config.h"
#include <limits>
#include "test_objects.h"
using namespace kmeans;


TEST_CASE("test_init_clusters", "[init]"){
    PPF_SIZE = 499;
    build_data();
    WKmeans<float> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);

    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(0, pdfs.size() - 1);
    //randomly select the first cluster
    int random_cluster_index = dist(gen);

    test_wkmeans.init_clusters();
    for (int i = 0; i < 4; i++){
        test_wkmeans.clusters[i].print();
    }
    PPF<float> first_cluster = ppfs[random_cluster_index];
    //testing first cluster
    REQUIRE(ppfs[random_cluster_index] == test_wkmeans.clusters[0]);

    //Getting the second cluster
    //creating the probability distribution
    float sum = 0.0;
    float distance{};
    std::vector<float> distances(pdfs.size());
    std::vector<float> prob(pdfs.size());
    for(int i =0; i < pdfs.size(); i ++){
        distance = wasserstein_2(first_cluster, ppfs[i]);
        distances[i] = distance*distance;
        sum += distance * distance;
    }
    for(int i =0; i < pdfs.size(); i++){
        prob[i] = distances[i]/sum;
    }
    std::discrete_distribution<> dist2(prob.begin(), prob.end());

    random_cluster_index = dist2(gen);

    PPF<float> second_cluster = ppfs[random_cluster_index];

    //second_cluster.print();

    REQUIRE(second_cluster == test_wkmeans.clusters[1]);
    for(int i = 0; i < pdfs.size(); i++)
        distances[i] = std::numeric_limits<float>::infinity();

    float min{};
    std::vector<PPF<float>> clusters;
    clusters.push_back(first_cluster);
    clusters.push_back(second_cluster);
    for(int i = 0; i < pdfs.size(); i++){
        for(PPF<float> qf: clusters){
            distance = wasserstein_2(ppfs[i], qf);
            float x = distance * distance;
            if(distances[i] > x){
                distances[i] = x;
            }
        }
    }
    sum = 0.0;
    for(int i = 0; i < pdfs.size(); i++){
        sum += distances[i];
    }
    for(int i =0; i < pdfs.size(); i++){
        prob[i] = distances[i]/sum;
    }
    std::discrete_distribution<> dist3(prob.begin(), prob.end());

    random_cluster_index = dist3(gen);

    PPF<float> third_cluster = ppfs[random_cluster_index];

    REQUIRE(third_cluster == test_wkmeans.clusters[2]);

}
TEST_CASE("test_init_clusters_monotone", "[init]"){
    PPF_SIZE = 50;
    build_data();
    WKmeans<float> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    for(int k = 0; k < N_CLUSTERS; k++){
        float prev_bin, curr_bin;
        prev_bin = test_wkmeans.clusters[k][0];
        for(int i = 1; i < PPF_SIZE; i++){
            curr_bin = test_wkmeans.clusters[k][i];
            REQUIRE(prev_bin <= curr_bin);
            prev_bin = curr_bin;
        }
    }

}
// in this function i need to test whether i am intitalizng the bounds properly, to do this i need to check if u(x) is properly assigned
TEST_CASE("test_init_bounds", "[init]"){
    build_data();
    WKmeans<float> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    //verify that the upperbounds of init_bounds is going to be the min cluster assignment of each datapoint
    std::vector<float> test_upper_bounds(pdfs.size(), std::numeric_limits<float>::infinity());
    std::vector<float> test_lower_bounds(pdfs.size() * N_CLUSTERS);
    std::vector<int> cluster_assignments(pdfs.size());
    for(int i = 0; i < pdfs.size(); i++){
        for(int k = 0; k < N_CLUSTERS; k ++){
            float distance = wasserstein_2(ppfs[i], test_wkmeans.clusters[k]);
            if(distance < test_upper_bounds[i]){
                test_upper_bounds[i] = distance;
                cluster_assignments[i] = k;
            }
        }
    }
    REQUIRE(test_upper_bounds == test_wkmeans.get_upper_bounds());
    //verify that the lowerbounds of each point in the 2d arary corresponds to the points data d(datapoint, and cluster)

    for(int i = 0; i < pdfs.size(); i++){
        for(int k = 0; k < N_CLUSTERS; k++){
            float d =  wasserstein_2(ppfs[i], test_wkmeans.clusters[k]);
            test_lower_bounds[i*N_CLUSTERS + k] = d;
        }
    }
    REQUIRE(test_lower_bounds == test_wkmeans.get_lower_bounds());
    REQUIRE(cluster_assignments == test_wkmeans.get_cluster_assignments());

    std::vector<int> cluster_size(N_CLUSTERS, 0);
    for(int i = 0; i < pdfs.size(); i++){
        cluster_size[cluster_assignments[i]] += 1;
    }

    REQUIRE(cluster_size == test_wkmeans.get_cluster_size());
}