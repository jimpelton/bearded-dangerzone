

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <bd/util/util.h>

#include <algorithm>
#include <vector>


TEST_CASE("to1D generates correct sequence", "[util][to1D]")
{
    std::vector<int> expected(3*4*5);
    int n{ 0 };
    std::generate(expected.begin(), expected.end(), [&n]() { return ++n; } );

    std::vector<int> actual;


    for( size_t z{ 0 }; z<3; ++z)
    for( size_t y{ 0 }; y<4; ++y)
    for( size_t x{ 0 }; x<5; ++x) {
        size_t idx{ bd::to1D(x, y, z, 5, 4) };
        actual.push_back(expected[idx]);
    }

    REQUIRE(expected == actual);
}




