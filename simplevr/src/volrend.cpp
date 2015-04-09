
#include "volrend.h"

#include <GLFW/glfw3.h>

#include <log/gl_log.h>

#include <vector>



///////////////////////////////////////////////////////////////////////////////////
// VolRend class
///////////////////////////////////////////////////////////////////////////////////

VolRend::VolRend(CommandLineOptions& cl)
    : m_cl(cl)
{   
}

VolRend::~VolRend() {}

bool VolRend::init(int scr_w, int src_h)
{
    if (m_context.init(this, scr_w, src_h) == nullptr) {
        gl_log("Could not initialize GLFW, exiting.");
        return false;
    }

    return true;
}

void VolRend::loop()
{   
}

///////////////////////////////////////////////////////////////////////////////////
// Callbacks
///////////////////////////////////////////////////////////////////////////////////

void VolRend::cursorpos_callback(double x, double y) {
    glm::vec2 cpos{ floor(x), floor(y) };

    if (glfwGetMouseButton(m_context.window(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        m_view.setMouseRotation(cpos);
    }

    m_view.setCursorPos(cpos);
}

void VolRend::keyboard_callback(int key, int scancode, int action, int mods)
{    
}


void VolRend::window_size_callback(int width, int height)
{
    m_view.setViewPort(width, height);
}

void VolRend::scrollwheel_callback(double xoff, double yoff)
{
    m_view.setPosOffset(xoff, yoff);
}

void VolRend::error_callback(int error, const char* description)
{
    
}

///////////////////////////////////////////////////////////////////////////////////
// Private members
///////////////////////////////////////////////////////////////////////////////////

void VolRend::makeBlockSlices(std::vector<QuadProto> &buffer)
{
    unsigned int n = m_cl.num_slices;

    if (n % 2 != 0)
        n += 1;

    buffer.resize(n);

    if (n == 1)
        return;

    //    for (int i = 0; i < n; ++i) {
    //        QuadProto::Quad &q = bufferZ[i].quad;
    //        
    //
    //    }
}


