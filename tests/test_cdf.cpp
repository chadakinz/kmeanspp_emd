#include <catch2/catch_test_macros.hpp>
#include "containers/cdf.h"

TEST_CASE("test_constructor", "[cdf]"){
    CDF<float> test_cdf1(5);
    REQUIRE(test_cdf1.size() == 5);
}