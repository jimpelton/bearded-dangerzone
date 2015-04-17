
#include <bd/log/gl_log.h>
#include <bd/util/context.h>
#include <bd/util/glfwcontext.h>


namespace bd {

// static
RenderLoop * Context::m_concon = nullptr;

// static
Context* Context::InitializeContext(RenderLoop *cc)
{
    gl_log_restart();
    gl_debug_log_restart();

    Context *context = new GlfwContext(cc);
    if (context) {
        bool success = context->init(1280, 720);
        context->isInit(success);
    }

    return context;
}


///////////////////////////////////////////////////////////////////////////////
Context::Context(RenderLoop *cc)
    : m_isInit{ false }
{
    m_concon = cc;
}


///////////////////////////////////////////////////////////////////////////////
Context::~Context()
{
    delete m_concon;
    m_concon = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
void Context::startLoop()
{
    m_concon->initialize(*this);
    m_concon->renderLoop();
}


///////////////////////////////////////////////////////////////////////////////
RenderLoop & Context::renderLoop()
{ 
    return *m_concon; 
}


///////////////////////////////////////////////////////////////////////////////
void Context::isInit(bool i) 
{
    m_isInit = i;
}


///////////////////////////////////////////////////////////////////////////////
bool Context::isInit() const 
{
    return m_isInit;
}

} // namespace bd
