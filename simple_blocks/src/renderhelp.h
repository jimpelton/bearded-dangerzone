//
// Created by Jim Pelton on 8/1/16.
//

#ifndef SUBVOL_RENDERHELP_H
#define SUBVOL_RENDERHELP_H


#include <glm/fwd.hpp>

namespace subvol
{
class Renderer;
class Camera;

extern Renderer* renderer;

void setInitialGLState();

/// \brief Set the camera at \c eye, looking at (0,0,0), with up vector (0,1,0).
//void setDefaultView(glm::vec3 const &eye);


} // namespace subvol


#endif //SUBVOL_GLRENDERHELP_H
