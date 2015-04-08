

#include "simplecontextcontroller.h"

#include <util/shader.h>
#include <graphics/vertexarrayobject.h>
#include <graphics/quad.h>

#include <glm/glm.hpp>

#include <utility>
#include <vector>




int main(int argc, const char *argv[])
{
    SimpleContextController scc;
    if (!scc.init(1280, 720)) {
        std::cerr << "Did not init context!" << std::endl;
        exit(1);
    }

    scc.renderLoop();

    return 0;
}








