
#include "datatypes.h"

namespace bearded { namespace dangerzone { namespace  file { 

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


} } }  /* namespace */