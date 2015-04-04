#ifndef translation_h__
#define translation_h__

#include <glm/glm.hpp>

namespace bd {

class Translation {
public:
    Translation();
    ~Translation();

    void translate(glm::vec3 to);
    void rotate(glm::vec3 axis, float amt);
    void scale(glm::vec3 by);

private:
    glm::mat4 translation;


};

} // namespace bd
#endif // translation_h__
