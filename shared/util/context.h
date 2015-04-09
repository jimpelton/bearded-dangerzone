#ifndef context_h__
#define context_h__

namespace bd {
;

class ContextController;

//TODO: add virtual window() method to Context.
class Context
{

public:

    static Context* InitializeContext(ContextController *cc);

    Context(ContextController *cc);
    virtual ~Context();

    void startLoop();
    

    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;
    virtual void init(int win_width, int win_height) = 0;

    static ContextController& Context::concon();

    bool isInit() const;

protected:
    void isInit(bool);
    

private:
    static ContextController *m_concon;
    bool m_isInit;

};


}


#endif // !context_h__
