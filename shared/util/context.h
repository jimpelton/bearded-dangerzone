#ifndef context_h__
#define context_h__

#include <util/contextcontroller.h>

namespace bd {
;


class Context
{
public:
    Context(ContextController *cc)
    {
        m_concon = cc;
    }

    virtual ~Context() 
    { 
    }

    //void startLoop() 
    //{
    //    
    //}

    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;

    static ContextController* concon() { return m_concon; }

private:
    static ContextController *m_concon;

};


}


#endif // !context_h__
