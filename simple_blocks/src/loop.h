//
// Created by jim on 4/2/17.
//

#ifndef SUBVOL_LOOP_H
#define SUBVOL_LOOP_H

#include <GLFW/glfw3.h>

#include "blockrenderer.h"
#include "timing.h"

namespace subvol
{
namespace renderhelp
{

class Loop
{
public:
  Loop();


  ~Loop();


  void
  loop(GLFWwindow *);


private:

  unsigned long long m_timeOfLastJob;

//  std::shared_ptr<BlockCollection> m_collection;

};


} // namespace renderhelp
} // namespace subvol

#endif // ! SUBVOL_LOOP_H
