#ifndef volrend_h__
#define volrend_h__

#include <GLFW/glfw3.h>

#include "cmdline.h"
#include "blockscollection.h"
#include "volume.h"

#include <bd/util/context.h>
#include <bd/util/contextcontroller.h>


#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>

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


    //////////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////////
    void makeBlockSlices(std::vector<glm::vec4> &vertices,
        std::vector<unsigned short> &indices);

    void makeVolumeRenderingShaders(const std::string &vert_path,
        const std::string &frag_path);

    //////////////////////////////////////////////////////////////////////////
    // Accessors
    //////////////////////////////////////////////////////////////////////////
    void window(GLFWwindow *w);

private:
    //////////////////////////////////////////////////////////////////////////
    // Private Data
    //////////////////////////////////////////////////////////////////////////
    CommandLineOptions m_cl;
    Volume m_vol;
    BlocksCollection m_col;
    GLFWwindow *m_window;

    //bd::VertexArrayObject m_quadproto;
    //const unsigned short restart{ 0xFFFF };
};

#endif
