//
// Created by jim on 4/2/17.
//

#include "loop.h"
#include "renderhelp.h"


#define MAX_MILLIS_SINCE_LAST_JOB 10000000

namespace subvol
{
namespace renderhelp
{

///////////////////////////////////////////////////////////////////////////////
Loop::Loop()
    : m_timeOfLastJob{ 0 }
{

}


///////////////////////////////////////////////////////////////////////////////
Loop::~Loop()
{

}


///////////////////////////////////////////////////////////////////////////////
void
Loop::loop(GLFWwindow *window)
{
  assert(window != nullptr && "window was passed as nullptr in loop()");
  bd::Info() << "About to enter render loop.";

  do {
    subvol::timing::startCpuTime();

    if (g_blockCollection->getRangeChanged()) {
      g_blockCollection->filterBlocks();
    }

    if (glfwGetTimerValue() - m_timeOfLastJob > MAX_MILLIS_SINCE_LAST_JOB) {
      // load some blocks to the m_gpu if any available.
      g_blockCollection->loadSomeBlocks();
      m_timeOfLastJob = glfwGetTimerValue();
    }

//  subvol::timing::startGpuTimerQuery();
    g_renderer->draw();
    glfwSwapBuffers(window);
//  subvol::timing::endGpuTimerQuery();

    glfwPollEvents();
    subvol::timing::endCpuTime();
  }
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0);

  bd::Info() << "Render loop exited.";
}


} // namespace renderhelp
} // namespace subvol
