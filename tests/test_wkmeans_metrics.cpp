#include <catch2/catch_test_macros.hpp>
#include "wkmeans/wkmeans.h"
#include "config.h"
#include "test_objects.h"
#include <catch2/catch_approx.hpp>
#include "distributions.hpp"
using namespace kmeans;
TEST_CASE("test_get_objective", "[metrics]"){
  PPF_SIZE = 500;
  build_data();
  double old_objective, f_objective;
  double new_objective = double{};

  // Store time series data for each file
  std::vector<std::vector<double>> all_objective_histories;

  for(int j = 0; j < 5; j++){
    std::vector<PDF<double>> pdfs;
    std::vector<CDF<double>> cdfs;
    std::vector<PPF<double>> ppfs;
    pdfs.reserve(6000);
    cdfs.reserve(6000);
    ppfs.reserve(6000);
    std::string file_name = "../tests/test_files/test_input_" + std::to_string(j + 1) +".txt";
    init_distributions(file_name, pdfs, cdfs, ppfs, 10);

    WKmeans<double> test_wkmeans(pdfs.size(), N_CLUSTERS, EPSILON, pdfs, cdfs, ppfs, 10, 42);
    test_wkmeans.init_clusters();
    test_wkmeans.init_bounds();
    f_objective = test_wkmeans.get_objective();
    test_wkmeans.update_clusters();
    test_wkmeans.update_bounds();
    test_wkmeans.swap_clusters();
    test_wkmeans.assign_new_clusters();
    new_objective = test_wkmeans.get_objective();

    // Store history for this file
    std::vector<double> objective_history;

    // Record initial state
    objective_history.push_back(f_objective);

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
      std::vector<int> cluster_assignments = test_wkmeans.get_cluster_assignments();


      // Record objective value at this iteration
      objective_history.push_back(new_objective);

      if(new_objective > old_objective){
        std::cout << "Warning: objective increased at iteration " << t
                  << " old: " << old_objective << " new: " << new_objective << std::endl;
        count ++;
      }
      REQUIRE(count < 2);
    }

    // Store histories for later plotting
    all_objective_histories.push_back(objective_history);

    std::cout << "File " << j+1 << " - first objective: " << f_objective
              << " last objective: " << new_objective << std::endl;
  }

  // Save data to CSV files for plotting
  for(size_t i = 0; i < all_objective_histories.size(); i++) {
    std::string filename = "../tests/inertia_metrics/convergence_data_file_" + std::to_string(i+1) + ".csv";
    std::ofstream outfile(filename);
    outfile << "inertia\n";

    for(size_t iter = 0; iter < all_objective_histories[i].size(); iter++) {
      outfile
              << all_objective_histories[i][iter] << "\n";
    }
    outfile.close();
    std::cout << "Saved convergence data to " << filename << std::endl;
  }
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