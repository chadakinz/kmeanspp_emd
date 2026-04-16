#include "config.h"
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "distributions.hpp"
#include "test_objects.h"
#include "wkmeans/wkmeans.h"
#include <catch2/catch_test_macros.hpp>

using namespace kmeans;

TEST_CASE("test_batch_procedure", "[batch]") {
  PPF_SIZE = 500;
  std::vector<PDF<double>> pdfs0;
  std::vector<CDF<double>> cdfs0;
  std::vector<PPF<double>> ppfs0;
  pdfs0.reserve(5000);
  cdfs0.reserve(5000);
  ppfs0.reserve(5000);
  init_distributions("../tests/test_files/test_sample_1.txt", pdfs0, cdfs0,
                     ppfs0, 10);

  std::cout << "distributions initialized, pdfs size: " << pdfs0.size()
            << std::endl;

  std::vector<uint32_t> sizes(N_CLUSTERS, 0);
  WKmeans<double> wkmeans_obj(pdfs0.size(), N_CLUSTERS, 1e-6, pdfs0, cdfs0,
                              ppfs0, 10, 0);
  wkmeans_obj.init_clusters();
  wkmeans_obj.run_batch_restart(sizes);
  std::cout << "ran batch restart" << std::endl;
  for (int i = 1; i < 25; i++) {
    std::vector<PPF<double>> previous_clusters = wkmeans_obj.clusters;
    std::vector<int> cluster_size = wkmeans_obj.get_cluster_size();
    for (int i = 0; i < sizes.size(); i++) {
      sizes[i] += cluster_size[i];
    }
    std::string file_name =
        "../tests/test_files/test_sample_" + std::to_string(i + 1) + ".txt";
    init_distributions_inplace(file_name, pdfs0, cdfs0, ppfs0, 10);

    wkmeans_obj.init_batch_kmeans(pdfs0, cdfs0, ppfs0, previous_clusters);
    wkmeans_obj.run_batch_restart(sizes);
  }
  std::vector<PDF<double>> pdfs4;
  std::vector<CDF<double>> cdfs4;
  std::vector<PPF<double>> ppfs4;
  pdfs4.reserve(50000);
  cdfs4.reserve(50000);
  ppfs4.reserve(50000);
  init_distributions("../tests/test_files/SAMPLE.txt", pdfs4, cdfs4, ppfs4, 10);
  WKmeans<double> test_wkmeans_obj(pdfs4.size(), N_CLUSTERS, 8e-4, pdfs4, cdfs4,
                                   ppfs4, 10, 0);

  double objective1 = test_wkmeans_obj.run_restart();

  test_wkmeans_obj.clusters = wkmeans_obj.clusters;

  test_wkmeans_obj.init_bounds();
  test_wkmeans_obj.assign_new_clusters();
  double objective2 = test_wkmeans_obj.get_objective();

  std::cout << "Objective for regular kmeans procedure " << objective1
            << std::endl;
  std::cout << "Objective for batch procedure " << objective2 << std::endl;
  REQUIRE(objective2 < objective1*(1.25));
}