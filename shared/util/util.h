#ifndef util_h__
#define util_h__

#include <glm/glm.hpp>

namespace bd {
    namespace util {
        void hsvToRgb(float h, float s, float v, glm::vec3 &rgb);
    }
} // namespace

#endif