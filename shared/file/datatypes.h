#ifndef datatypes_h__
#define datatypes_h__

#include <map>
#include <string>

namespace bd {
    namespace file {
        enum class DataType
        {
            Integer,
            UnsignedInteger,

            Character,
            UnsignedCharacter,

            Short,
            UnsignedShort,

            Float
        };

        extern const std::map<std::string, DataType> DataTypesMap;
    }
} /* namespace */

#endif  /* ifndef datatypes_h__ */
