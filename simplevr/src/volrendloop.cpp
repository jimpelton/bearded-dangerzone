
#include "volrendloop.h"

#include <bd/log/gl_log.h>

#include <bd/graphics/vertexarrayobject.h>
#include <bd/graphics/quad.h>
#include <bd/util/shader.h>

#include <GLFW/glfw3.h>

#include <vector>

namespace 
{
    float m_mouseSpeed = 1.0f;
    glm::vec2 m_cursorPos;
    bd::Shader m_volshader_vertex{ bd::ShaderType::Vertex };
    bd::Shader m_volshader_fragment{ bd::ShaderType::Fragment };
    bd::ShaderProgram m_volshader_program;
}


//////////////////////////////////////////////////////////////////////////
VolRendLoop::VolRendLoop(const CommandLineOptions &cl)
    : m_cl(cl)
    , m_vol{  }
    , m_window{ nullptr }
{   
}


//////////////////////////////////////////////////////////////////////////
VolRendLoop::~VolRendLoop() {}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::renderLoop(bd::Context &c)
{   
    
    
}

///////////////////////////////////////////////////////////////////////////////////
// Callbacks
///////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::cursorpos_callback(double x, double y) {
    glm::vec2 cpos{ floor(x), floor(y) };

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        ///////  OBJECT ROTATION LEFT BUTTON  ///////
        glm::vec2 delta{ cpos - m_cursorPos };

        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * m_mouseSpeed,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * m_mouseSpeed,
            glm::vec3(0, 1, 0)
            );

        m_vol.transform().rotate(rotX * rotY);

    } else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        ///////  CAMERA ROTATION RIGHT BUTTON  ///////
        glm::vec2 delta{ cpos - m_cursorPos };

        glm::quat rotX = glm::angleAxis<float>(
            glm::radians(delta.y) * m_mouseSpeed,
            glm::vec3(1, 0, 0)
            );

        glm::quat rotY = glm::angleAxis<float>(
            glm::radians(delta.x) * m_mouseSpeed,
            glm::vec3(0, 1, 0)
            );

        view().rotate(rotX * rotY);
    }
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::keyboard_callback(int key, int scancode, int action, int mods)
{    
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::window_size_callback(int width, int height)
{
    view().setViewport(0, 0, width, height);
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::scrollwheel_callback(double xoff, double yoff)
{
    
}


//////////////////////////////////////////////////////////////////////////
void VolRendLoop::window(GLFWwindow *w)
{
    m_window = w;
}

//////////////////////////////////////////////////////////////////////////
void VolRendLoop::makeBlockSlices(std::vector<glm::vec4> &vertices,
    std::vector<unsigned short> &indices)
{
    using bd::Quad;
    unsigned int n = m_cl.num_slices;
    size_t vertexCount, indexCount;

    if (n <= 1) {  
        // n = 0 or 1
        n = 1;
    } else if (n % 2 != 0) {  
        // n odd
        n += 1;
    }
    
    vertexCount = n * Quad::verts.size();
    indexCount = n + n * Quad::verts.size();

    vertices.clear();
    vertices.reserve(vertexCount);
    indices.clear();
    indices.reserve(indexCount);

    for (size_t i = 0; i < vertexCount; i+=4) {
        vertices.push_back(bd::Quad::verts[0]);
        vertices.push_back(bd::Quad::verts[1]);
        vertices.push_back(bd::Quad::verts[2]);
        vertices.push_back(bd::Quad::verts[3]);
    }

    for (size_t i = 0; i < indexCount; i+=5) {
        indices.push_back(bd::Quad::elements[0 * i]);
        indices.push_back(bd::Quad::elements[1 * (i+1)]);
        indices.push_back(bd::Quad::elements[2 * (i+2)]);
        indices.push_back(bd::Quad::elements[3 * (i+3)]);
        indices.push_back(static_cast<unsigned short>(0xFFFF));
    }

    gl_log("Created %d slices, &d vertices, &d indices.",
           n, vertexCount, indexCount);

}


void VolRendLoop::makeVolumeRenderingShaders(const std::string &vert_path, 
    const std::string &frag_path)
{
    m_volshader_vertex.loadFromFile(vert_path);
    m_volshader_fragment.loadFromFile(frag_path);

    unsigned int shaderId = m_volshader_program.linkProgram(&m_volshader_vertex, &m_volshader_fragment);
    if (shaderId == 0) {
        gl_log_err("Could not create volume rendering shader program, id returned was 0.");
    } else {
        gl_log("Created volume rendering shader program, id=%d", shaderId);
    }

}


