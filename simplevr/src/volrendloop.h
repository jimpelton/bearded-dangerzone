#ifndef volrend_h__
#define volrend_h__

#include "cmdline.h"
#include "blockscollection.h"
#include "volume.h"

#include <bd/util/contextcontroller.h>
#include <bd/graphics/shader.h>
#include <bd/graphics/vertexarrayobject.h>

#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>


class VolRendLoop : public bd::ContextController
{
public:
    VolRendLoop();
    virtual ~VolRendLoop();


    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    void initialize(bd::Context &) override;
    void renderLoop() override;
    void cursorpos_callback(double x, double y) override;
    void keyboard_callback(int key, int scancode, int action, int mods) override;
    void window_size_callback(int width, int height) override;
    void scrollwheel_callback(double xoff, double yoff) override;


    //////////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////////
    void addVbaContext(const std::vector<glm::vec4> &verts,
       const std::vector<unsigned short> &indices);

    void makeVolumeRenderingShaders(const std::string &vert_path,
        const std::string &frag_path);

    //////////////////////////////////////////////////////////////////////////
    /// \brief Set the root object in the scene graph, probably the volume.
    //////////////////////////////////////////////////////////////////////////
    void root(bd::Transformable *v);

private:

    void drawBlocks();

    //////////////////////////////////////////////////////////////////////////
    // Private Data
    //////////////////////////////////////////////////////////////////////////

    float m_mouseSpeed; //{ 1.0f };
    glm::vec2 m_cursorPos;

    bd::Transformable *m_vol;

    bd::Shader m_volshader_vertex;
    bd::Shader m_volshader_fragment;
    bd::ShaderProgram m_volshader_program;

    bd::VertexArrayObject m_slices_vao;
};

#endif
