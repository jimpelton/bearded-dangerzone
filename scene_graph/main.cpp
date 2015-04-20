//
// Created by jim on 4/16/15.
//

#include "simplerenderloop.h"

#include <bd/scene/transformable.h>
#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/quad.h>
#include <bd/util/context.h>
#include <bd/util/renderloop.h>
#include <bd/log/gl_log.h>

#include <vector>



//std::vector<glm::vec4> qverts(bd::Quad::verts.begin(), bd::Quad::verts.end());
//std::vector<unsigned short> elems(bd::Quad::elements.begin(), bd::Quad::elements.end());
//
//m_vao.addVbo(qverts, 0);
//m_vao.setIndexBuffer(elems);
//m_vao.addVbo(qcolors, 1);
//
//m_vao.bind();


void makeTree()
{

}

int main(int argc, char* argv[])
{
    SimpleRenderLoop * scc = new SimpleRenderLoop();
    bd::Context *context = bd::Context::InitializeContext(scc);

    if (!context) {
        gl_log_err("Unable to initialize SimpleRenderLoop...exiting.");
        exit(1);
    }

//    scc->setRoot(&root);

    context->startLoop();

    return 0;
}