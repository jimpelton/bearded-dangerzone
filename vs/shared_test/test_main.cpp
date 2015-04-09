

#include "simplecontextcontroller.h"

#include <util/shader.h>
#include <graphics/vertexarrayobject.h>
#include <graphics/quad.h>
#include <util/context.h>
#include <util/glfwcontext.h>

#include <glm/glm.hpp>

#include <utility>
#include <vector>


int main(int argc, const char *argv[])
{
    SimpleContextController scc;
    bd::Context *con = bd::Context::InitializeContext(&scc);
    bd::GlfwContext *gcon = reinterpret_cast<bd::GlfwContext*>(con);
    
    scc.setWindow(gcon->window());

    if (! gcon->isInit()) {
        std::cerr << "Did not init context!" << std::endl;
        exit(1);
    }

    gcon->startLoop();

    return 0;
}








