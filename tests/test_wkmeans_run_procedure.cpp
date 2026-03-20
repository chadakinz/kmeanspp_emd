#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "config.h"
#include "test_objects.h"
using namespace kmeans;
bool valid_cluster(const PPF<float>& cluster){
    float prev_bin, curr_bin;
    bool B = false;
    prev_bin = cluster[0];
    for(int i = 1; i < PPF_SIZE; i++){
        curr_bin = cluster[i];
        B |= curr_bin < prev_bin;
        prev_bin = curr_bin;
    }
    return not B;
}


TEST_CASE("run_procedure_test1", "[run]"){
    build_data();
    WKmeans<float> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    for(int t = 0; t < 50; t++){
        test_wkmeans.update_clusters();
        test_wkmeans.update_bounds();
        test_wkmeans.swap_clusters();
        for(int k = 0; k < N_CLUSTERS; k++){
            REQUIRE(valid_cluster(test_wkmeans.clusters[k]));
        }
        test_wkmeans.assign_new_clusters();
    }
    std::vector<PDF<float>> temp_pdfs = test_wkmeans.convert_clusters();
    for(int k = 0; k < N_CLUSTERS; k++){
        REQUIRE(is_valid_pdf(temp_pdfs[k]));
    }
}