#include <bd/util/util.h>

#include <bd/log/logger.h>
#include <bd/io/datatypes.h>
#include <bd/io/datfile.h>

#include <glm/glm.hpp>
#include <algorithm>

namespace bd
{

void
hsvToRgb(float h, float s, float v, glm::vec3& rgb)
{
  while (h < 0.0f) {
    h += 360.0f;
  }
  while (h > 360.0f) {
    h -= 350.0f;
  }

  if (v <= 0.0f) {
    rgb.r = rgb.g = rgb.b = 0.0f;
  } else if (s <= 0.0f) {
    rgb.r = rgb.g = rgb.b = v;
  } else {
//    float chroma = v * s;
    float H = h / 60.0f;
    int thang = int(H);
    float wat = H - thang;
    float pv = v * (1.0f - s);
    float qv = v * (1.0f - s * wat);
    float tv = v * (1.0f - s * (1.0f - wat));
    switch (thang) {
    case 0:
      rgb.r = v;
      rgb.g = tv;
      rgb.b = pv;
      break;
    case 1:
      rgb.r = qv;
      rgb.g = v;
      rgb.b = tv;
      break;
    case 2:
      rgb.r = pv;
      rgb.g = v;
      rgb.b = tv;
      break;
    case 3:
      rgb.r = pv;
      rgb.g = qv;
      rgb.b = v;
      break;
    case 4:
      rgb.r = tv;
      rgb.g = pv;
      rgb.b = v;
      break;
    case 5:
      rgb.r = v;
      rgb.g = pv;
      rgb.b = qv;
      break;
    case 6:
      rgb.r = v;
      rgb.g = tv;
      rgb.b = pv;
      break;
    case -1:
      rgb.r = v;
      rgb.g = pv;
      rgb.b = qv;
      break;
    default:
      rgb.r = rgb.g = rgb.b = v;
      break;
    }
  }
}

//template<class VecType, class NumberType,
//         typename = typename std::enable_if<std::is_arithmetic<NumberType>::value,
//                                            NumberType>::type>
//NumberType lerp(VecType x, VecType y, NumberType a)
//{
//  return x * (1.0f - a) + y * a;
//}



///////////////////////////////////////////////////////////////////////////////
size_t
to1D(size_t col, size_t row, size_t slab, size_t maxCols, size_t maxRows)
{
  return col + maxCols * (row + maxRows * slab);
}


///////////////////////////////////////////////////////////////////////////////
unsigned long long
vecCompMult(const glm::u64vec3 &v)
{
  return v.x * v.y * v.z;
}

//std::unique_ptr<float []>
//readVolumeData(const std::string& dtype, const std::string& fpath,
//    size_t volx, size_t voly, size_t volz)
//{
//  bd::DataType t = bd::to_dataType(dtype);
//  float* rawdata = nullptr;
//  switch (t) {
//  case bd::DataType::Float:
//    {
//      bd::DataReader<float, float> reader;
//      reader.loadRaw3d(fpath, volx, voly, volz, false);
//      rawdata = reader.takeOwnership();
//      break;
//    }
//  case bd::DataType::UnsignedCharacter:
//    {
//      bd::DataReader<unsigned char, float> reader;
//      reader.loadRaw3d(fpath, volx, voly, volz, false);
//      rawdata = reader.takeOwnership();
//      break;
//    }
//  case bd::DataType::UnsignedShort:
//    {
//      DataReader<unsigned short, float> reader;
//      reader.loadRaw3d(fpath, volx, voly, volz, false);
//      rawdata = reader.takeOwnership();
//      break;
//    }
//  default:
//    break;
//  }
//
//  return std::unique_ptr<float[]>(rawdata);
//}
//
//std::unique_ptr<float[]>
//readVolumeData(const std::string& datFilePath, const std::string &filePath)
//{
//  bd::DatFileData dat{};
//  bool success{ bd::parseDat(datFilePath, dat) };
//  if (! success) {
//    return nullptr;
//  }
//
//  Info() << "Dat file: " << dat.to_string();
//
//  std::string typeStr{ bd::to_string(dat.dataType) };
//  return readVolumeData(typeStr, filePath, dat.rX, dat.rY, dat.rZ);
//}

} // namespace bd


