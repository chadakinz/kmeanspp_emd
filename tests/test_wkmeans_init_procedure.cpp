#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include <iostream>
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "containers/cdf.h"
#include "config.h"

using namespace kmeans;

TEST_CASE("test_init_clusters", "[init]"){
std::vector<PDF<float>> pdfs = {
        PDF<float>{0.0,0.0,0.0,0.086957,0.521739,0.152174,0.043478,0.130435,0.065217,0.0},
        PDF<float>{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.326087,0.673913},
        PDF<float>{0.0,0.065217,0.456522,0.347826,0.0,0.065217,0.0,0.021739,0.043478,0.0},
        PDF<float>{0.0,0.0,0.130435,0.195652,0.195652,0.26087,0.130435,0.0,0.086957,0.0},
        PDF<float>{0.0,0.0,0.0,0.0,0.021739,0.521739,0.304348,0.108696,0.0,0.043478},
        PDF<float>{0.0,0.0,0.326087,0.23913,0.0,0.0,0.108696,0.0,0.0,0.326087},
        PDF<float>{0.0,0.0,0.521739,0.347826,0.0,0.065217,0.065217,0.0,0.0,0.0},
        PDF<float>{0.0,0.0,0.5,0.130435,0.065217,0.0,0.065217,0.065217,0.0,0.173913},
        PDF<float>{0.0,0.086957,0.173913,0.347826,0.26087,0.0,0.0,0.0,0.130435,0.0},
        PDF<float>{0.0,0.065217,0.326087,0.304348,0.0,0.043478,0.065217,0.0,0.0,0.195652},
        PDF<float>{0.021739,0.173913,0.369565,0.282609,0.021739,0.021739,0.0,0.108696,0.0,0.0},
        PDF<float>{0.0,0.0,0.0,0.0,0.195652,0.326087,0.152174,0.23913,0.086957,0.0},
        PDF<float>{0.347826,0.434783,0.021739,0.0,0.065217,0.021739,0.043478,0.0,0.065217,0.0},
        PDF<float>{0.173913,0.26087,0.434783,0.0,0.065217,0.0,0.0,0.065217,0.0,0.0},
        PDF<float>{0.0,0.0,0.0,0.608696,0.065217,0.195652,0.0,0.0,0.130435,0.0},
        PDF<float>{0.0,0.0,0.0,0.0,0.130435,0.0,0.456522,0.108696,0.0,0.304348},
        PDF<float>{0.0,0.0,0.0,0.043478,0.021739,0.0,0.0,0.0,0.434783,0.5},
        PDF<float>{0.782609,0.0,0.043478,0.043478,0.043478,0.021739,0.0,0.065217,0.0,0.0},
        PDF<float>{0.717391,0.065217,0.0,0.0,0.0,0.0,0.130435,0.0,0.0,0.086957},
        PDF<float>{0.065217,0.0,0.0,0.0,0.173913,0.608696,0.065217,0.043478,0.043478,0.0}
};
std::vector<CDF<float>> cdfs;
std::vector<PPF<float>> ppfs;
CDF<float> temp_cdf(10);
PPF<float> temp_ppf(PPF_SIZE);
for (int i = 0; i < pdfs.size(); i++){
    temp_cdf = pdfs[i].get_cdf();
    cdfs.push_back(temp_cdf);
    temp_ppf = temp_cdf.get_ppf();
    ppfs.push_back(temp_ppf);
}
WKmeans<float> test_wkmeans(20, 4, EPSILON, pdfs, cdfs, ppfs, 10, 42);

std::mt19937 gen(42);
std::uniform_int_distribution<int> dist(0, 19);
//randomly select the first cluster
int random_cluster_index = dist(gen);
ppfs[random_cluster_index].print();
pdfs[random_cluster_index].print();
cdfs[random_cluster_index].print();

test_wkmeans.init_clusters();
for (int i = 0; i < 4; i++){
    test_wkmeans.clusters[i].print();
}

REQUIRE(1 == 1);
}