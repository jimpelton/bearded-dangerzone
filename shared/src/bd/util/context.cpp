
#include <bd/log/gl_log.h>
#include <bd/util/context.h>
#include <bd/util/glfwcontext.h>


namespace bd {
;

// static
ContextController* Context::m_concon = nullptr;

// static
Context* Context::InitializeContext(ContextController *cc)
{
    Context *context = new GlfwContext(cc);
    if (context) {
        context->init(1280, 720);
    }

    return context;
}


///////////////////////////////////////////////////////////////////////////////
Context::Context(ContextController *cc)
    : m_isInit{ false }
{
    gl_log_restart();
    gl_debug_log_restart();
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
    m_concon->renderLoop(*this);
}


///////////////////////////////////////////////////////////////////////////////
ContextController& Context::concon()
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
