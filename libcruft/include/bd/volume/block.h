#ifndef block_h__
#define block_h__

#include <bd/graphics/texture.h>
#include <bd/io/fileblock.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

namespace bd
{

//////////////////////////////////////////////////////////////////////////
/// \brief Associates volumetric data in voxel space with a location
/// in world coordinates.
/// A blocks transform can be used to position a set of proxy geometry in
/// 3D world space. The block's texture contains the GL id/name of the 3D
/// texture that should be sampled by the proxy geometry.
//////////////////////////////////////////////////////////////////////////
class Block
{

public:


  ///////////////////////////////////////////////////////////////////////////////
  /// \brief Create block (i,j,k) with specified dims and world origin.
  /// \param[in] ijk The block ID (and position with in the grid of blocks).
  /// \param[in] dims The world dimensions in of this block.
  /// \param[in] fb This block's FileBlock.
  Block(glm::u64vec3 const &ijk, FileBlock const &fb);


  virtual
  ~Block();


  void
  sendToGpu();


//  bool
//  visible() const;


//  void
//  visible(bool);


  /// \brief Get the FileBlock for this block.
  const FileBlock&
  fileBlock() const;


  uint64_t
  index() const;


  /// \brief Get the ijk location of this block.
  const glm::u64vec3 &
  ijk() const;


  /// \brief Set the ijk location of this block.
  void
  ijk(glm::u64vec3 const & ijk);


  /// \brief Set if this block is marked empty.
  void empty(bool);
  /// \brief Get if this block is marked empty.
  bool
  empty() const;


  /// \brief Get the average value of this here block.
  double
  avg() const;


  /// \brief Get the center world coordinates of this block.
  glm::vec3 const &
  origin() const;


  glm::vec3 const &
  worldDims() const;

  /// \brief Get the texture assoc'd with this block.
  Texture* 
  texture() ;


  void
  texture(Texture *tex);

  /// \brief Remove the Block's texture.
  /// Same as calling texture(nullptr)
  bd::Texture *
  removeTexture();

  /// \brief Get a reference to this blocks model-to-world transform matrix.
  glm::mat4&
  transform();


  /// \brief Construct a vector containing the dimensions of this block in voxels.
  glm::u64vec3
  voxel_extent() const;


  size_t
  byteSize() const;


  int
  status() const;


  char* 
  pixelData();


  void
  pixelData(char *data);

  char*
  removePixelData();

  /// \brief String rep. of this blockeroo.
  std::string
  to_string() const;

  static const int CLEAR     = 0x00;
  static const int NOT_EMPTY = 0x01;
  static const int GPU_RES   = 0x02;
  static const int GPU_WAIT  = 0x04;
  static const int CPU_RES   = 0x08;
  static const int CPU_WAIT  = 0x10;

  /**
    statuses:
     0 -- CLEAR                        (Block is not in cpu/gpu cache's and is not visible)
     1 -- VISIBLE                      (Visible, waiting to be loaded from disk)
     9 -- VISIBLE & CPU_RES            (In cpu cache, waiting for texture, waiting in load queue)
     8 -- CPU_RES                      (non-vis, but still in cpu cache)
    10 -- GPU_RES & CPU_RES            (non-vis, but still in gpu/cpu cache)
    11 -- VISIBLE & GPU_RES  & CPU_RES (Ready for drawing, has been uploaded to GPU)
    13 -- VISIBLE & GPU_WAIT & CPU_RES (Has texture, waiting in render thread loadables queue)
   */


private:
  FileBlock m_fb;        ///< The FileBlock (contains the info from IndexFile)
  glm::u64vec3 m_ijk;    ///< Block's location in block coordinates.
  glm::vec3 m_origin;    ///< This blocks center in world coordinates.
  glm::vec3 m_worldDims; ///< The size of this block in world coords.
  glm::mat4 m_transform; ///< Block's model-to-world transform matrix.

  Texture *m_tex ; ///< Texture assoc'd with this block.
  char *m_pixelData; ///< CPU resident texture data (nullptr if non-resident).

  ///
  /// 0x01 -- visible.
  /// 0x02 -- resident in GPU memory
  /// 0x04 -- resident in CPU memory
  /// 0x04 --
  int m_status;

  bool m_isVisible;

}; // class Block

/// \brief Output a string representation of given Block.
std::ostream& operator<<(std::ostream& os, const Block& b);

} // namespace bd

#endif // !block_h__


