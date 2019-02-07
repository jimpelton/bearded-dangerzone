//
// Created by Jim Pelton on 9/28/16.
//


#include <bd/volume/transferfunction.h>

#include <catch.hpp>
#include <iostream>

#define RES_DIR RESOURCE_FOLDER

namespace {


} // namespace

TEST_CASE("OTF can parse a correct file", "[otf][parse]")
{
  const char* tf_path = RES_DIR "/scalar_opacity_tf.1dt";
  bd::OpacityTransferFunction otf{ };
  otf.load(tf_path);

  std::vector<bd::OpacityKnot> const
      knots_actual { { 0.00, 0.00 },
                     { 0.16, 0.00 },
                     { 0.33, 1.00 },
                     { 0.50, 1.00 },
                     { 0.66, 1.00 },
                     { 0.83, 0.50 },
                     { 1.00, 0.00 } };

  REQUIRE(otf.getKnotsVector() == knots_actual);

}

TEST_CASE("OTF returns alpha value on exact match", "[otf][interpolate]")
{
  const char* tf_path = RES_DIR "/scalar_opacity_tf.1dt";
  bd::OpacityTransferFunction otf{ };
  otf.load(tf_path);

  double val{ 0.0 };

  val = otf.interpolate(0.0);
  REQUIRE(val == 0.00);

  val = otf.interpolate(0.16);
  REQUIRE(val == 0.00);

  val = otf.interpolate(0.5);
  REQUIRE(val == 1.00);

  val = otf.interpolate(0.66);
  REQUIRE(val == 1.0);

  val = otf.interpolate(0.83);
  REQUIRE(val == 0.50);

  val = otf.interpolate(1.00);
  REQUIRE(val == 0.0);

}

TEST_CASE("interpolate returns interpolated values", "[otf][interpolate]")
{
  const char* tf_path = RES_DIR "/scalar_opacity_tf.1dt";
  bd::OpacityTransferFunction otf{ };
  otf.load(tf_path);

  double val{ 0.0 };

  val = otf.interpolate(0.755);
  REQUIRE(val == 0.7205882353);

  val = otf.interpolate(0.55);
  REQUIRE(val == 1.0);

  val = otf.interpolate(0.05);

}
