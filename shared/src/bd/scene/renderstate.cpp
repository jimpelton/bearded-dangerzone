
#include <GL/glew.h>

#include <bd/log/gl_log.h>
#include <bd/scene/renderstate.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>
#include <iostream>

namespace bd {

RenderState::RenderState(VertexArrayObject *vao, ShaderProgram *program)
    : m_vao{ vao }, m_progy{ program }
{
}


RenderState::~RenderState()
{
}

void RenderState::draw()
{

    gl_check(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    switch (m_vao->method()) {
        case VertexArrayObject::Method::ELEMENTS:
            drawElementArrays();
            break;
        case VertexArrayObject::Method::ARRAYS:
            drawArrays();
            break;
        default:
            break;
    }

}

void RenderState::bind()
{
    m_vao->bind();
    m_progy->bind();
}

void RenderState::unbind()
{
    m_progy->unbind();
    m_vao->unbind();
}

void RenderState::drawElementArrays()
{
    gl_check(glDrawElements(GL_TRIANGLE_STRIP, m_vao->numElements(),
        GL_UNSIGNED_SHORT, 0));
}

void RenderState::drawArrays()
{

}


} // namespace bd
