#include <catch2/catch_test_macros.hpp>
#include "containers/cdf.h"

//todo: refactor test cases to reflect using reimann midpoint instead of reimann left
TEST_CASE("test_cdf", "[cdf]"){
    CDF<float> test_cdf1(5);
    REQUIRE(test_cdf1.size() == 5);
    CDF<float> test_cdf2 =  {0.09, 0.36, 0.88, 1.00};
    CDF<float> test_cdf2_ppf = {0, .25, .25, .25, .25, .25, .25, .50, .50, .50, .50, .50, .50, .50,.50,.50,.50, .75, .75, .75};
    REQUIRE(test_cdf2_ppf == test_cdf2.get_ppf());
    CDF<float> test_cdf3 = {0, 0, 0.521739, 0.869565, 0.869565, 0.934782, 0.999999, 0.999999 ,0.999999, 0.999999};
    CDF<float> test_cdf3_ppf = {.2, .2, .2, .2, .2, .2, .2, .2, .2, .2, .3, .3, .3, .3, .3, .3, .3, .5, .6, .6};
    REQUIRE(test_cdf3_ppf == test_cdf3.get_ppf());
}
