#ifndef volrend_h__
#define volrend_h__

#include "cmdline.h"
#include "blockscollection.h"
#include "volume.h"

#include <bd/util/contextcontroller.h>
#include <bd/util/glfwcontext.h>
//#include <graphics/view.h>
#include <bd/graphics/vertexarrayobject.h>

#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>

class VolRendLoop : public bd::ContextController
{
public:
    VolRendLoop(const CommandLineOptions &);
    virtual ~VolRendLoop();

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    void renderLoop(bd::Context &) override;
    void cursorpos_callback(double x, double y) override;
    void keyboard_callback(int key, int scancode, int action, int mods) override;
    void window_size_callback(int width, int height) override;
    void scrollwheel_callback(double xoff, double yoff) override;

    void window(GLFWwindow *w);

private:
    
    void makeBlockSlices(std::vector<glm::vec4> &buffer,
        std::vector<unsigned short> &indices);
    void makeVolumeRenderingShaders(const std::string &vert_path,
        const std::string &frag_path);

    //////////////////////////////////////////////////////////////////////////
    // Private Data
    //////////////////////////////////////////////////////////////////////////

    CommandLineOptions m_cl;
    Volume m_vol;
    BlocksCollection m_col;
    GLFWwindow *m_window;

    bd::VertexArrayObject m_quadproto;

    const unsigned short restart{ 0xFFFF };

};

#endif
