//
// Created by jim on 10/3/16.
//

#include <bd/io/fileblock.h>

#include <iomanip>
#include <sstream>

namespace bd
{
///////////////////////////////////////////////////////////////////////////////
std::string
FileBlock::to_string() const
{
  std::stringstream ss;
  ss <<
     "   \"block_" << block_index << "\": {\n"
      "      \"index\": " << block_index << ",\n"
      "      \"ijk_index\": [" << ijk_index[0] << ", " << ijk_index[1] << ", "
     << ijk_index[2] << "],\n"
      "      \"data_offset\": " << data_offset << ",\n"
      "      \"data_bytes\": " << data_bytes << ",\n"
      "      \"voxel_dims\": [" << voxel_dims[0] << ", " << voxel_dims[1] << ", "
     << voxel_dims[2] << "],\n"
      "      \"world_dims\": [" << world_dims[0] << ", " << world_dims[1] << ", "
     << world_dims[2] << "],\n"
      "      \"world_oigin\": [" << world_oigin[0] << ", " << world_oigin[1] << ", "
     << world_oigin[2] << "],\n"
      "      \"min_val\": " << std::fixed << min_val << ",\n"
      "      \"max_val\": " << std::fixed << max_val << ",\n"
      "      \"avg_val\": " << std::fixed << avg_val << ",\n"
      "      \"total_val\": " << std::fixed << total_val << ",\n"
      "      \"rov\": " << std::fixed << rov << ",\n"
      "      \"empty_voxels\": " << empty_voxels << ",\n"
      "      \"empty\": " << std::boolalpha << is_empty << "\n"
      "   }";

  return ss.str();
}

} //namespace bd
