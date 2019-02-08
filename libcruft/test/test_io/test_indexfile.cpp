//
// Created by jim on 2/11/17.
//


#include <bd/io/fileblock.h>
#include <bd/io/indexfile/indexfile.h>

#include <catch.hpp>


TEST_CASE("I don't know yet")
{
  bd::IndexFile index_file;
  bd::Volume *v{ &index_file.getVolume() };
  v->block_count({3, 3, 3});
  v->voxelDims({256, 256, 256});
  index_file.init(bd::DataType::Character);

  std::vector<bd::FileBlock> const &blocks{ index_file.getFileBlocks() };

  REQUIRE(blocks.size() == 3*3*3);

  bd::FileBlock const *block = &blocks[0];
  auto *thing = block->ijk_index;
  REQUIRE(thing[0] == 0);
  REQUIRE(thing[1] == 0);
  REQUIRE(thing[2] == 0);

  block = &blocks[1];
  thing = block->ijk_index;
  REQUIRE(thing[0] == 1);
  REQUIRE(thing[1] == 0);
  REQUIRE(thing[2] == 0);

  block = &blocks[2];
  thing = block->ijk_index;
  REQUIRE(thing[0] == 2);
  REQUIRE(thing[1] == 0);
  REQUIRE(thing[2] == 0);

  block = &blocks[3];
  thing = block->ijk_index;
  REQUIRE(thing[0] == 0);
  REQUIRE(thing[1] == 1);
  REQUIRE(thing[2] == 0);


  block = &blocks[26];
  thing = block->ijk_index;
  REQUIRE(thing[0] == 2);
  REQUIRE(thing[1] == 2);
  REQUIRE(thing[2] == 2);

}

TEST_CASE("Byte offsets are correct")
{
  bd::IndexFile index_file;
  bd::Volume *v{ &index_file.getVolume() };
  v->block_count({ 2, 2, 2 });
  v->voxelDims({ 256, 256, 256 });
  index_file.init(bd::DataType::Character);

  std::vector<bd::FileBlock> const &blocks{ index_file.getFileBlocks() };

  bd::FileBlock const *block{ nullptr };

  block = &blocks[0];
  uint64_t offset = block->data_offset;
  REQUIRE(offset == 0);

  block = &blocks[1];
  offset = block->data_offset;
  REQUIRE(offset == 128);

  block = &blocks[2];
  offset = block->data_offset;
  REQUIRE(offset == 2 * 128 * 128);

  block = &blocks[3];
  offset = block->data_offset;
  REQUIRE(offset == (2 * 128 * 128) + 128);

  block = &blocks[4];
  offset = block->data_offset;
  REQUIRE(offset == 256 * 256 * 128);

  block = &blocks[5];
  offset = block->data_offset;
  REQUIRE(offset == (256 * 256 * 128) + 128);

  block = &blocks[6];
  offset = block->data_offset;
  REQUIRE(offset == (256 * 256 * 128) + (256*128));

  block = &blocks[7];
  offset = block->data_offset;
  REQUIRE(offset == (256 * 256 * 128) + (256 * 128) + 128);
}
