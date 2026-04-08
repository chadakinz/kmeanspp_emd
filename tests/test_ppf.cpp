#include <catch2/catch_test_macros.hpp>
#include "containers/ppf.h"
#include "config.h"
#include "containers/pdf.h"
TEST_CASE("test_ppf", "[ppf]"){
    PPF_SIZE = 100;
    PDF<double> test_pdf1 = {0.0,0.086957,0.173913,0.347826,0.26087,0.0,0.0,0.0,0.130435,0.0};
    PDF<double> test_pdf2 = {0.0,0.065217,0.326087,0.304348,0.0,0.043478,0.065217,0.0,0.0,0.195652};

    CDF<double> test_cdf1 = test_pdf1.get_cdf();
    PPF<double> test_ppf1 = test_cdf1.get_ppf();

    CDF<double> test_cdf2 = test_pdf2.get_cdf();
    PPF<double> test_ppf2 = test_cdf2.get_ppf();
    std::cout << wasserstein_2(test_ppf1, test_ppf2);
    CDF<double> test_cdf3 = test_ppf2.get_cdf(10);
    REQUIRE(1 == 1);


}