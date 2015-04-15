
#include <bd/log/gl_log.h>
#include <bd/util/context.h>
#include <bd/util/glfwcontext.h>


namespace bd {

// static
ContextController* Context::m_concon = nullptr;

// static
Context* Context::InitializeContext(ContextController *cc)
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
Context::Context(ContextController *cc)
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
