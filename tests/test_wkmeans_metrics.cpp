#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include "config.h"
#include "test_objects.h"

using namespace kmeans;
TEST_CASE("test_get_objective", "[metrics]"){
    build_data();
    float old_objective, new_objective;
    WKmeans<float> test_wkmeans(pdfs.size(), 4, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    test_wkmeans.update_clusters();
    test_wkmeans.update_bounds();
    test_wkmeans.swap_clusters();
    test_wkmeans.assign_new_clusters();
    new_objective = test_wkmeans.get_objective();
    for(int t = 0; t < 50; t++){
        old_objective = new_objective;
        test_wkmeans.update_clusters();
        test_wkmeans.update_bounds();
        test_wkmeans.swap_clusters();
        test_wkmeans.assign_new_clusters();
        new_objective = test_wkmeans.get_objective();
        REQUIRE(new_objective <= old_objective);
    }
}