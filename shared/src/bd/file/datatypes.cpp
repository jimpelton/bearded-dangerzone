#include <bd/file/datatypes.h>

namespace bd {

const std::map<std::string, DataType> DataTypesMap
{
    { "int", DataType::Integer },
    { "unsigned integer", DataType::UnsignedInteger },

    { "char", DataType::Character },
    { "uchar", DataType::UnsignedCharacter },
    { "unsigned char", DataType::UnsignedCharacter },

    { "short", DataType::Short },
    { "ushort", DataType::UnsignedShort },
    { "unsigned short", DataType::UnsignedShort },

    { "float", DataType::Float }
};

} /* namespace bd */

