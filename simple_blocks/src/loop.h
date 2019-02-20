//
// Created by jim on 4/2/17.
//

#ifndef SUBVOL_LOOP_H
#define SUBVOL_LOOP_H

#include <GLFW/glfw3.h>
#include "blockrenderer.h"
#include "timing.h"
#include "axis_enum.h"
#include "io/blockcollection.h"

#include <bd/graphics/renderer.h>
#include <memory>
namespace subvol
{
namespace renderhelp
{

class Loop
{
public:
  Loop(GLFWwindow *, std::shared_ptr<bd::Renderer> r,
       std::shared_ptr<subvol::BlockCollection> c);


  virtual
  ~Loop();


  void
  loop();


protected:

  virtual void
  tick(float time);


  double
  timeNow() const;

protected:
  GLFWwindow *_window;
  std::shared_ptr<bd::Renderer> _renderer;
  std::shared_ptr<BlockCollection> _collection;
  uint64_t _frameCount;

private:
  float m_timeOfLastJob;
  double const m_tf;
  uint32_t m_numFrames;

};

class BenchmarkLoop
    : public Loop
{
public:
  BenchmarkLoop(GLFWwindow *, std::shared_ptr<BlockRenderer>,
                std::shared_ptr<BlockCollection>,
                glm::vec3 rAxis = { 1, 0, 0 });


  virtual ~BenchmarkLoop();


protected:
  void
  tick(float time) override;


private:
  float m_rotateSpeed;
  float m_lastFrameTime;
  float m_timeOfLastJob;
  glm::vec3 m_rotationAxis;


};

} // namespace renderhelp
} // namespace subvol

#endif // ! SUBVOL_LOOP_H
