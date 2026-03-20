#include <catch2/catch_test_macros.hpp>
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
    std::ifstream file("./test_files/medium_input.txt");
    std::string line;
    std::getline(file, line);
    PDF<float> test_pdf1(10);

    process_line_into_pdf<float>(line, 10, test_pdf1);
    REQUIRE(is_valid_pdf(test_pdf1));
    PDF<float> test_pdf1_test = {0.0, 0.0, 0.0, 0.086957 ,0.521739 ,0.152174,0.043478 ,0.130435, 0.065217, 0.0};
    REQUIRE(test_pdf1 == test_pdf1_test);

}
TEST_CASE("test_init_distribution", "[file]"){
    std::vector<PDF<float>> pdfs;
    std::vector<CDF<float>> cdfs;
    std::vector<PPF<float>> ppfs;
    pdfs.reserve(6000);
    cdfs.reserve(6000);
    ppfs.reserve(6000);
    init_distributions("./test_files/medium_input.txt", pdfs, cdfs, ppfs, 10);
    for(int i = 0; i < 6000; i++){
        pdfs[i].print();
        std::cout << i << std::endl;
        REQUIRE(is_valid_pdf(pdfs[i]));
    }
}