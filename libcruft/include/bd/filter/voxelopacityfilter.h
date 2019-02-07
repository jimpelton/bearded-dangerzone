//
// Created by Jim Pelton on 8/31/16.
//

#ifndef bd_voxelopacityfilter_h
#define bd_voxelopacityfilter_h

#include <bd/volume/transferfunction.h>

#include <limits>
#include <vector>

namespace bd
{

template<class Ty>
class VoxelOpacityFilter
{

public:
  /// \brief Create a filter based on the given opacity function.
  /// \note Scalars in OpacityKnots should be normalized data values.
  VoxelOpacityFilter(std::vector<OpacityKnot> const & function,
      double knotMin,
      double knotMax,
      Ty const & dataMin,
      Ty const & dataMax)
      : m_func{ function }
      , m_min{ knotMin }
      , m_max{ knotMax }
      , m_dataMin{ dataMin }
      , m_diff{ static_cast<Ty>(dataMax-dataMin) }
  {

  }


  ~VoxelOpacityFilter()
  {
  }


  bool operator()(Ty const& val) const
  {
    double val_norm{ ( val-m_dataMin )/static_cast<double>(m_diff) };
    double a{ alpha(val_norm) };
    return a>=m_min && a<m_max;

  }


  // find the alpha value associated with scalar value v.
  // v assumed between 0..1.
  double alpha(double v) const
  {
    OpacityKnot b{ 0, 0 };
    OpacityKnot a{ m_func[0] };
    for (size_t i = 1; i<m_func.size(); ++i) {
      b = m_func[i];

      //TODO: fix unsafe comparison 
      if (v==b.s) {
        return b.alpha;
      } else if (v<b.s) {
        // v is between a.scalar and b.scalar, so lerp the alpha value.
        return a.alpha*( 1.0-v )+b.alpha*v;
      }

      a = m_func[i];
    }

    return 0;

  }


private:
  std::vector<OpacityKnot> const m_func;
  double const m_min;
  double const m_max;
  Ty const m_dataMin;
  Ty const m_diff;


}; // class VoxelOpacityFilter

} // namespace bd

#endif // ! voxelopacityfilter_h
