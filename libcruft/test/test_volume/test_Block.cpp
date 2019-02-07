//
// Created by jim on 2/11/17.
//


#include <bd/volume/block.h>
#include <bd/io/fileblock.h>
#include <bd/graphics/texture.h>


#include <catch.hpp>


namespace {

}

TEST_CASE("empty() returns false if is_empty is 1.", "[block][empty]")
{

  bd::FileBlock fb;

  SECTION("Block is empty") {
    fb.is_empty = 1;
    bd::Block b({0,0,0}, fb);

    REQUIRE(b.empty() == true);
  }

  SECTION("Block is not empty.") {
    fb.is_empty = 0;
    bd::Block b({0,0,0}, fb);

    REQUIRE(b.empty() == false);
  }
}


TEST_CASE("empty(bool)", "[block][empty]")
{
  bd::FileBlock fb;
  auto originalFBValue = fb.is_empty;
  bd::Block b{{0,0,0}, fb};


  SECTION("new block does not change FileBlock empty status in c'tor")
  {
    REQUIRE(originalFBValue == b.fileBlock().is_empty);
  }

  SECTION("new block empty() returns whatever is the fb status")
  {
    REQUIRE(b.fileBlock().is_empty == b.empty());
  }

  SECTION("empty(false) sets is_empty=0")
  {
    b.empty(false);
    REQUIRE(b.fileBlock().is_empty == 0);
    REQUIRE(b.status() == 0x1); // NOT_EMPTY
  }

  SECTION("empty(true) sets status visible")
  {
    b.empty(true);
    REQUIRE(b.fileBlock().is_empty == 1);
    REQUIRE(b.status() == 0x0); // CLEAR
  }
}


TEST_CASE("status set correct by texture()", "[block][texture]")
{
  bd::Block b{{0,0,0}, {}};
  bd::Texture t{ bd::Texture::Target::Tex3D };

  SECTION("new block as 0 status")
  {
    REQUIRE(b.status() == 0x0);
  }


  SECTION("non-null texture sets status as GPU_WAIT")
  {
    REQUIRE(b.texture() == nullptr);
    b.texture(&t);
    REQUIRE(b.texture() == &t);
    REQUIRE(b.status() == 0x04);
  }

  SECTION("null texture clears GPU_WAIT status")
  {
    b.texture(&t);
    REQUIRE(b.texture());
    REQUIRE(b.status() == 0x04);
    b.texture(nullptr);
    REQUIRE(b.status() == 0x00);
  }

}

TEST_CASE("block has status 13 after setting the stuff", "[block][status]")
{

  bd::Block b{{0,0,0}, {}};
  char c = 'a';
  bd::Texture t{ bd::Texture::Target::Tex3D };

  REQUIRE(b.status() == 0x00); // 0 CLEAR

  b.empty(false);
  REQUIRE(b.status() == 0x01); // 1 NE

  b.pixelData(&c);
  REQUIRE(b.status() == 0x09); // 9 NE & CPU_RES

  b.texture(&t);
  REQUIRE(b.status() == 0xd); // 13 NE & CPU_RES & GPU_WAIT

  b.removeTexture();
  REQUIRE(b.status() == 0x09); // 9 NE & CPU_RES

  b.pixelData(nullptr);
  REQUIRE(b.status() == 0x01); // 1 NE

  b.empty(true);
  REQUIRE(b.status() == 0x00); // 0 CLEAR
}