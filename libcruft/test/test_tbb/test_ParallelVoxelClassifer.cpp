//
// Created by Jim Pelton on 9/4/16.
//

#include <bd/volume/transferfunction.h>
#include <bd/filter/voxelopacityfilter.h>
#include <bd/tbb/parallelfor_voxelrelevance.h>
#include <bd/filter/valuerangefilter.h>


#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>


#include <fstream>
#include <iostream>

#include <catch.hpp>

#define RES_DIR RESOURCE_FOLDER

namespace
{

const std::vector<bd::OpacityKnot> func{
    { 0.0,  0.0 },
    { 0.10, 0.1 },
    { 0.50, 0.8 },
    { 0.60, 0.9 },
    { 1.0,  1.0 } };

const bd::VoxelOpacityFilter<unsigned char> vof{
    func,
    0.8, 1.0,
    static_cast<unsigned char>(0),
    static_cast<unsigned char>(255) };

} // namespace




/*
 * This test reads the first 64 bytes from testvol_8x8x8.raw.
 * The data are unsigned char, values are 0 to 189.
 *    Offset   |   Values
 *    (bytes)  |
 *    ---------+-----------------------------------------------------------------
 *    0000000  |    0   3   6   9  12  15  18  21  24  27  30  33  36  39  42  45
 *    0000016  |   48  51  54  57  60  63  66  69  72  75  78  81  84  87  90  93
 *    0000032  |   96  99 102 105 108 111 114 117 120 123 126 129 132 135 138 141
 *    0000048  |  144 147 150 153 156 159 162 165 168 171 174 177 180 183 186 189
 * The test should find 1's for values (128 .. 167)
*/
TEST_CASE("ParallelVoxelClassifer test")
{
  const char* dir = RES_DIR "/testvol_8x8x8.raw";
  std::ifstream in(dir, std::ios::binary);
  if (! in.is_open()) {
    FAIL("Could not open testvol_8x8x8.raw");
  }

  std::vector<char> contents(64, 0);
  in.read(contents.data(), 64);
  in.close();

  auto rel = [](unsigned char val) -> bool {
    return val > 128 && val < 168;
  };

  std::vector<bool> map(contents.size(), false);

  bd::Buffer<unsigned char> buf{
      reinterpret_cast<unsigned char*>(contents.data()), contents.size() };

  bd::ParallelForVoxelRelevance<unsigned char, decltype(rel), std::vector<bool>> classifier{ map, &buf, rel };

  tbb::blocked_range<size_t> range{ 0, contents.size() };
  tbb::parallel_for(range, classifier);

  std::cout << "done";

  for(int i{ 0 }; i < 43; ++i) {
    if (map[i] == true) {
      FAIL("At index " + std::to_string(i) + " map was true, should have been false.");
    }
  }

  for(int i{ 43 }; i < 56; ++i) {
    if (map[i] == false) {
      FAIL("At index " + std::to_string(i) + " map was false, should have been true.");
    }
  }

  for(int i{ 56 }; i < 64; ++i) {
    if (map[i] == true) {
      FAIL("At index " + std::to_string(i) + " map was true, should have been false.");
    }
  }

  REQUIRE(true);
}
