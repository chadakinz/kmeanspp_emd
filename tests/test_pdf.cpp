#include <catch2/catch_test_macros.hpp>
#include "containers/pdf.h"

TEST_CASE("test_pdf", "[pdf]"){
    PDF<double> test_pdf1(5);
    REQUIRE(test_pdf1.size() == 5);

    PDF<double> test_pdf2 = {0.25f, 0.25f, 0.25f, 0.25f};

    CDF<double> test_pdf_cdf = {.25, .5, .75, 1};

    REQUIRE(test_pdf2.get_cdf() == test_pdf_cdf);
}