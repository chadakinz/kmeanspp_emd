#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "wkmeans/wkmeans.h"
#include <iostream>
#include "containers/pdf.h"
#include "containers/ppf.h"
#include "containers/cdf.h"
#include "config.h"
#include <limits>
#include "distributions.hpp"
#include "test_objects.h"

TEST_CASE("test_process_line", "[file]"){
    std::ifstream file("../tests/test_files/medium_input.txt");
    std::string line;
    std::getline(file, line);
    PDF<double> test_pdf1(10);

    process_line_into_pdf<double>(line, 10, test_pdf1);
    REQUIRE(is_valid_pdf(test_pdf1));
    PDF<double> test_pdf1_test = {0.0, 0.0, 0.0, 0.086957 ,0.521739 ,0.152174,0.043478 ,0.130435, 0.065217, 0.0};
    for (size_t i = 0; i < test_pdf1.size(); i++) {
        REQUIRE(test_pdf1[i] == Catch::Approx(test_pdf1_test[i]).epsilon(1e-7));
    }

}
TEST_CASE("test_init_distribution", "[file]"){
    std::vector<PDF<double>> pdfs;
    std::vector<CDF<double>> cdfs;
    std::vector<PPF<double>> ppfs;
    pdfs.reserve(6000);
    cdfs.reserve(6000);
    ppfs.reserve(6000);
    init_distributions("../tests/test_files/medium_input.txt", pdfs, cdfs, ppfs, 10);
    for(int i = 0; i < 6000; i++){
        REQUIRE(is_valid_pdf(pdfs[i]));
    }
}