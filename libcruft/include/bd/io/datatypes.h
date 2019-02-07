#ifndef datatypes_h__
#define datatypes_h__

#include <map>
#include <string>

namespace bd
{

enum class DataType
{
  Integer,
  UnsignedInteger,

  Character,
  UnsignedCharacter,

  Short,
  UnsignedShort,

  Float,
  Double,

  Unknown
};

//extern const std::map<std::string, DataType> DataTypesMap;

DataType
to_dataType(const std::string &typeStr);

std::string
to_string(DataType);

size_t
to_sizeType(DataType);

} /* namespace bd */

#endif /* ifndef datatypes_h__ */

