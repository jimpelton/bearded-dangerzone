#ifndef volrend_h__
#define volrend_h__

#include "cmdline.h"
#include "blockscollection.h"

#include <util/contextcontroller.h>
#include <util/glfwcontext.h>

#include <glm/gtc/quaternion.hpp>
#include "volume.h"

class View
{
public:
    void setMouseRotation(const glm::vec2 &cpos);
    
    void setCursorPos(const glm::vec2 &cpos);

    void setViewPort(int w, int h);

    void setPosOffset(double xoff, double yoff);

    void updateTransform();

private:
    int m_screenWidth;
    int m_screenHeight;
    
    float m_mouseSpeed;
    float m_fov;

    glm::vec2 m_cursorPos;

    glm::vec3 m_position;
    glm::vec3 m_focus;
    glm::vec3 m_up;
    
    glm::vec4 m_viewDir;
    
    glm::quat m_rotation;

    glm::mat4 m_proj;
    glm::mat4 m_view;
    glm::mat4 m_transform;

    bool m_viewDirty;
};

class VolRend : public bd::ContextController
{
public:
    explicit VolRend(CommandLineOptions &cl);
    virtual ~VolRend();

    bool init(int scr_w, int scr_h) override;
    void loop();

    virtual void cursorpos_callback(double x, double y) override;
    virtual void keyboard_callback(int key, int scancode, int action, int mods) override;
    virtual void window_size_callback(int width, int height) override;
    virtual void scrollwheel_callback(double xoff, double yoff) override;

    virtual void error_callback(int error, const char *description);


private:
    
    struct QuadProto;
    void makeBlockSlices(std::vector<QuadProto> &);


private:
    CommandLineOptions &m_cl;
    bd::GlfwContext m_context;
    View m_view;
    Volume m_vol;
    BlocksCollection m_col;

private:
    struct QuadProto
    {
        struct Quad{
            glm::vec4 ll{ -0.5f, -0.5f, 0.0f, 1.0f };
            glm::vec4 lr{ 0.5f, -0.5f, 0.0f, 1.0f };
            glm::vec4 ul{ -0.5f, 0.5f, 0.0f, 1.0f };
            glm::vec4 ur{ 0.5f, 0.5f, 0.0f, 1.0f };
        } quad;

        const unsigned int restart{ 0xFFFF };
    };

};

#endif
