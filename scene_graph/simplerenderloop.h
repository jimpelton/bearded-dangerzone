
#ifndef simplerenderloop_h__
#define simplerenderloop_h__

#include <bd/util/renderloop.h>
#include <bd/scene/view.h>
#include <bd/scene/transformable.h>

#include <iostream>



class SimpleRenderLoop : public bd::RenderLoop
{
public:

    SimpleRenderLoop();

    virtual ~SimpleRenderLoop();


    virtual void renderLoop() override;

    virtual void cursorpos_callback(double x, double y) override;

    virtual void keyboard_callback(int key, int scancode, int action, int mods) override;

    virtual void window_size_callback(int width, int height) override;

    virtual void scrollwheel_callback(double xoff, double yoff) override;

    virtual void error_callback(int error, const char *description);

    virtual void initialize(bd::Context &context) override;

    void setRoot(bd::Transformable *);

private:
    double getTime();

    bd::Transformable *m_root;


    const std::string m_vertStr =
        "#version 400\n"
        "in vec3 vert;"
        "in vec3 col;"
        "uniform mat4 mvp;"
        "out vec3 color;"
        "void main() { "
        "    gl_Position = mvp * vec4(vert, 1.0f);"
        "    color = col;"
        "}";

    const std::string m_fragStr =
        "#version 400\n"
        "in vec3 color;"
        "out vec4 out_col;"
        "void main() {"
        "    out_col = vec4(color, 1.0f);"
        //"    out_col = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
        "}";
};

#endif
