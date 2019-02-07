//
// Created by Jim Pelton on 4/26/16.
//

#ifndef bd_volume_h__
#define bd_volume_h__

#include <glm/glm.hpp>
#include <bd/log/logger.h>


namespace bd
{
class Volume
{
public:
  Volume();


  Volume(glm::u64vec3 const &voxelDims, glm::u64vec3 const &numBlocks);


  Volume(Volume const &other);


  ~Volume();


  /// \brief Get/set the number of voxels in each block.
  const glm::u64vec3&
  block_dims() const;


  /// \brief Get/Set the number of blocks along each axis.
  glm::u64vec3 const&
  block_count() const;


  /// \brief Get/Set the number of blocks along each axis.
  void
  block_count(glm::u64vec3 const &);

  /// \brief Calculate the total number of blocks in this volume.
  uint64_t
  total_block_count() const;


  /// \brief Get/Set the volume's dimensions in voxels
  const glm::u64vec3&
  voxelDims() const;


  /// \brief Set the voxel dimensions of this volume.
  void
  voxelDims(glm::u64vec3 const &voxdims);


  /// \brief Get the voxel dimensions of this volume.
  glm::u64vec3 const&
  voxelDims();


  /// \brief Get the volume world dimensions.
  /// This is supposed be a value between 0 and 1 along each axis,
  /// with the longest axis being 1.
  glm::f32vec3 const&
  worldDims() const;

  /// \brief Set the world dimensions of the volume.
  /// This should be a value between 0 and 1 along each axis,
  /// with the longest axis being 1.
  void
  worldDims(glm::f32vec3 const &);


  /// \brief Get the minimum data value in the volume
  double
  min() const;


  /// \brief Set the minimum data value in the volume
  void
  min(double);


  /// \brief Get the maximum data value in the volume
  double
  max() const;


  /// \brief Set the maximum data value in the volume
  void
  max(double);


  /// \brief Get the average data value in the volume
  double
  avg() const;

  /// \brief Set the average data value in the volume
  void
  avg(double);

  /// \brief Get the sum of the data values in the volume.
  double
  total() const;

  /// \brief Set the sum of the data values in the volume
  void
  total(double);


  /// \brief Set the min rov of the volume.
  void
  rovMin(double);


  /// \brief Get the min rov of the volume.
  double
  rovMin() const;


  /// \brief Set the max rov of the volume.
  void
  rovMax(double);


  /// \brief Get the max rov value of the voxels in the volume.
  double
  rovMax() const;

  /// \brief Get the number of empty voxels in the volume
  /// \note This is not accurate value anymore.
  uint64_t
  numEmptyVoxels() const;


  /// \brief Set the number of empty voxels in the volume
  /// \note This is not accurate value anymore.
  void
  numEmptyVoxels(uint64_t);


  /// \brief Return the voxel extent covered by the blocks.
  ///
  /// If volume voxel dimensions are not divisible by the number of
  /// blocks, then the blocks may not span the entire extent of the
  /// volume's voxels.
  ///
  /// \return x, y, z dims of the voxel extent.
  glm::u64vec3
  blocksExtent() const;


private:

  glm::u64vec3 m_blockDims; ///< Dimensions of a block in this volume, in voxels.
  glm::u64vec3 m_blockCount; ///< Number of equal sized blocks the volume is divided into.
  glm::u64vec3 m_voxelDims; ///< Volume dimensions in voxels.
  glm::f32vec3 m_worldDims; ///< Volume dimensions in world coords.
  uint64_t m_volEmptyVoxels; ///< Number of non-relevant voxels.
  double m_volMax; ///< Max value found in volume.
  double m_volMin; ///< Min value found in volume.
  double m_volAvg; ///< Avg value found in volume.
  double m_volTot; ///< Total of all values in volume.
  double m_rovMin; ///< Min rov value found in the volume.
  double m_rovMax; ///< Max rov value found in the volume.
};


std::ostream&
operator<<(std::ostream &os, bd::Volume const &);

} // namespace bd

#endif // ! bd_volume_h__
