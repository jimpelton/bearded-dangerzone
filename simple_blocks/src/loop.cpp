//
// Created by jim on 4/2/17.
//

#include "loop.h"
#include "renderhelp.h"


#define MAX_SECONDS_SINCE_LAST_JOB 1 //0000000

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
  double const time_freq{ 1.0 / glfwGetTimerFrequency() };

  do {
    subvol::timing::startCpuTime();


    if (glfwGetTimerValue()*time_freq - m_timeOfLastJob > MAX_SECONDS_SINCE_LAST_JOB) {
      // load some blocks to the m_gpu if any available.
      g_blockCollection->loadSomeBlocks();
      m_timeOfLastJob = glfwGetTimerValue()*time_freq;
    }

    if (g_blockCollection->getRangeChanged()) {
      g_blockCollection->filterBlocks();
    }

    g_renderer->draw();
    glfwSwapBuffers(window);

    glfwPollEvents();
    subvol::timing::endCpuTime();
  }
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
      glfwWindowShouldClose(window) == 0);

  bd::Info() << "Render loop exited.";
}


} // namespace renderhelp
} // namespace subvol
