#ifndef volrend_h__
#define volrend_h__

#include "cmdline.h"
#include "blockscollection.h"
#include "volume.h"

#include <util/contextcontroller.h>
#include <util/glfwcontext.h>

#include <glm/gtc/quaternion.hpp>


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


    CommandLineOptions &m_cl;
    bd::GlfwContext m_context;
    //View m_view;
    Volume m_vol;
    BlocksCollection m_col;

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
