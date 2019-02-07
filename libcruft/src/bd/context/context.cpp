#include <bd/log/gl_log.h>
#include <bd/log/logger.h>
#include <bd/context/context.h>
#include <bd/context/glfwcontext.h>


namespace bd
{
// static
RenderLoop* Context::m_loop = nullptr;

// static
Context*
Context::InitializeContext(RenderLoop* cc)
{

  Context* context = new GlfwContext(cc);
  bool success = context->init(1280, 720);
  context->isInit(success);

  return context;
}


///////////////////////////////////////////////////////////////////////////////
Context::Context(RenderLoop* cc)
  : m_isInit{ false }
{
  m_loop = cc;
}


///////////////////////////////////////////////////////////////////////////////
Context::~Context()
{
  if (m_loop) delete m_loop;
}


///////////////////////////////////////////////////////////////////////////////
void
Context::startLoop()
{
  Info() << "Context initializing renderloop.";
  m_loop->initialize(*this);

  Info() << "Starting render loop.";
  do {
    m_loop->render();
    swapBuffers();
    pollEvents();
  } while (!windowShouldClose());

  Info() << "Renderloop exited.";
}


///////////////////////////////////////////////////////////////////////////////
RenderLoop&
Context::renderLoop()
{
  return *m_loop;
}


///////////////////////////////////////////////////////////////////////////////
void
Context::isInit(bool i)
{
  m_isInit = i;
}


///////////////////////////////////////////////////////////////////////////////
bool
Context::isInit() const
{
  return m_isInit;
}
} // namespace bd


