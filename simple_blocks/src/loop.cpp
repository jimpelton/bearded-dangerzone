//
// Created by jim on 4/2/17.
//

#include "loop.h"
#include "renderhelp.h"


#define MAX_SECONDS_SINCE_LAST_JOB 1.0 //0000000

namespace subvol
{
namespace renderhelp
{



///////////////////////////////////////////////////////////////////////////////
Loop::Loop(GLFWwindow *window, std::shared_ptr<BlockRenderer> r,
     std::shared_ptr<BlockCollection> c)
  : _window{ window }
  , _renderer{ r }
  , _collection{ c }
  , m_timeOfLastJob{ 0 }
  , m_tf{ 1.0 / glfwGetTimerFrequency() }
{
}


///////////////////////////////////////////////////////////////////////////////
Loop::~Loop()
{

}


///////////////////////////////////////////////////////////////////////////////
void
Loop::loop()
{
  assert(window != nullptr && "window was passed as nullptr in loop()");
  bd::Info() << "About to enter render loop.";



  do {
    _frameCount++;
    tick(timeNow());

  } while (glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(_window) == 0);

  bd::Info() << "Render loop exited.";
}

void 
Loop::tick(float now)
{
  m_numFrames++;
  if (now - m_timeOfLastJob > MAX_SECONDS_SINCE_LAST_JOB) {
    // load some blocks to the m_gpu if any available.
    _collection->loadSomeBlocks();
    m_numFrames = 0;
    m_timeOfLastJob = timeNow();
  }

  if (_collection->getRangeChanged()) {
    _collection->filterBlocks();
  }

  _renderer->draw();

  glfwSwapBuffers(_window);
  glfwPollEvents();
}


  double
  Loop::timeNow() const
{
  return static_cast<double>(glfwGetTimerValue()) * m_tf;
}




BenchmarkLoop::BenchmarkLoop(GLFWwindow *window, std::shared_ptr<BlockRenderer> r,
                             std::shared_ptr<BlockCollection> c,
                             glm::vec3 rAxis)
  : Loop(window, r, c)
  , m_rotateSpeed{ 120.0f }
  , m_lastFrameTime{ 0.0f }
  , m_timeOfLastJob{ 0.0f }
  , m_rotationAxis{ rAxis }
{
}

BenchmarkLoop::~BenchmarkLoop()
{
}

void
BenchmarkLoop::tick(float now)  // override
{
  // render the blocks
  Loop::tick(now);
 
  double dt{ now - m_lastFrameTime };
  bd::Camera &cam{ _renderer->getCamera() };
  glm::mat4 const rotation{ glm::rotate(glm::mat4{ 1 }, glm::radians(float(dt)*m_rotateSpeed), m_rotationAxis) };
  cam.setEye(rotation * glm::vec4{ cam.getEye(), 1 });
  cam.setUp(rotation * glm::vec4{ cam.getUp(), 1 });
  _renderer->setViewMatrix(cam.createViewMatrix());
  
  m_lastFrameTime = now;
}

} // namespace renderhelp
} // namespace subvol
