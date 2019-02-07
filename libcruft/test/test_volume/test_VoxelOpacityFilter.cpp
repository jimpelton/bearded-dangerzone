//
// Created by Jim Pelton on 9/4/16.
//

#include <bd/filter/voxelopacityfilter.h>
#include <bd/volume/transferfunction.h>

#include <vector>
#include <limits>

#include <catch.hpp>

namespace
{

const std::vector<bd::OpacityKnot> func{
    { 0.0,  0.0 },
    { 0.10, 0.1 },
    { 0.50, 0.8 },
    { 0.60, 0.9 },
    { 1.0,  1.0 }
};

const bd::VoxelOpacityFilter<unsigned char>
    vof{ func,
         0.8, 1.0,
         static_cast<unsigned char>(0),
         static_cast<unsigned char>(255) };

} // namespace

TEST_CASE("Given value between 0 and 1 return interpolated alpha", "[voxelopacityfilter][alpha]")
{
  double a{ vof.alpha(0.55) };
// should get 0.855
  REQUIRE(a == 0.855);
}

TEST_CASE("Given first value return interpolated alpha", "[voxelopacityfilter][alpha]")
{
  double a{ vof.alpha(0.0) };
  REQUIRE(a == 0.0);
}

TEST_CASE("Given last value return interpolated alpha", "[voxelopacityfilter][alpha]")
{
  double a{ vof.alpha(1.0) };
  REQUIRE(a == 1.0);
}

TEST_CASE("Given some matching value return interpolated alpha", "[voxelopacityfilter][alpha]")
{
  double a{ vof.alpha(0.60) };
  REQUIRE(a == 0.9);
}


TEST_CASE("Given relevant voxel, returns true", "[voxelopacityfilter][operator()]")
{
  bool r{ vof(130) };
  REQUIRE(r == true);
}

TEST_CASE("Given matching relevant voxel, returns true", "[voxelopacityfilter][operator()]")
{
  bool r{ vof(128) };
  REQUIRE(r == true);
}

TEST_CASE("Given irrelevant voxel, returns false", "[voxelopacityfilter][operator()]")
{
  bool r{ vof(100) };
  REQUIRE(r == false);
}

TEST_CASE("Given matching irrelevant voxel, returns false", "[voxelopacityfilter][operator()]")
{
  bool r{ vof(0) };
  REQUIRE(r == false);
}
