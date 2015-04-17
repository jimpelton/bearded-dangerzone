//
// Created by jim on 4/16/15.
//

#include "simplecontextcontroller.h"

#include <bd/scene/transform.h>
#include <bd/scene/transformable.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/quad.h>
#include <bd/util/context.h>
#include <bd/util/contextcontroller.h>
#include <bd/log/gl_log.h>

#include <vector>


bd::VertexArrayObject vao;
bd::Transformable root;



void makeTree()
{

}

int main(int argc, char* argv[])
{
    SimpleContextController * scc = new SimpleContextController();
    bd::Context *context = bd::Context::InitializeContext(scc);

    if (!context) {
        gl_log_err("Unable to initialize SimpleContextController...exiting.");
        exit(1);
    }


    std::vector<glm::vec4> vertices;

    std::vector<unsigned short> indices;
    std::vector<glm::vec4> qverts(bd::Quad::verts.begin(), bd::Quad::verts.end());
    std::vector<unsigned short> elems(bd::Quad::elements.begin(), bd::Quad::elements.end());

    const std::vector<glm::vec3> qcolors{
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 0.0, 0.0, 1.0 }
    };

    vao.addVbo(qverts, 0);
    vao.setIndexBuffer(elems);
    vao.addVbo(qcolors, 1);
    scc->setRoot(&root);

    context->startLoop();


    return 0;
}