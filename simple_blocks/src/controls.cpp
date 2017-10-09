//
// Created by jim on 8/12/16.
//

#include "controls.h"
#include "renderhelp.h"
#include "colormap.h"

namespace subvol
{

Controls *Controls::s_instance{ nullptr };


Controls::Controls()
    : m_cursor{ }
    , m_renderer{ nullptr }
    , m_showBlockBoxes{ false }
    , m_shouldUseLighting{ false }
    , m_currentBackgroundColor{ 0 }
    , m_scaleValue{ 1.0f }
    , m_nShiney{ 1.0f }
    , m_LightVector{ 1.0f, 1.0f, 1.0f }
{

}


Controls::~Controls()
{
}


void
Controls::initialize(std::shared_ptr<BlockRenderer> renderer)
{
  if (s_instance == nullptr) {
    s_instance = new Controls();
  }

  assert(renderer != nullptr && "Passed nullptr as argument to initialize");
  Controls &inst{ getInstance() };
  inst.m_shouldUseLighting = renderer->getShouldUseLighting();
  inst.m_renderer = std::move(renderer);
}


/* static */
Controls &
Controls::getInstance()
{
  assert(s_instance != nullptr &&
             "Controls::initiralize() should be called prior to first call of getInstance");

  return *s_instance;
}


void
Controls::s_cursorpos_callback(GLFWwindow *window, double x, double y)
{
  assert(s_instance != nullptr);
  s_instance->cursorpos_callback(window, x, y);
}


void
Controls::s_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  assert(s_instance != nullptr);
  s_instance->keyboard_callback(key, scancode, action, mods);
}


void
Controls::s_window_size_callback(GLFWwindow *window, int width, int height)
{
  assert(s_instance != nullptr);
  s_instance->window_size_callback(width, height);
}


void
Controls::s_scrollwheel_callback(GLFWwindow *window, double xoff, double yoff)
{
  assert(s_instance != nullptr);
  s_instance->scrollwheel_callback(xoff, yoff);
}


void
Controls::s_mousebutton_callback(GLFWwindow *window, int button, int action, int mods)
{
  assert(s_instance != nullptr);
  s_instance->mousebutton_callback(window, button, action, mods);
}


void
Controls::cursorpos_callback(GLFWwindow *window, double x, double y)
{
  glm::vec2 cpos(std::floor(x), std::floor(y));

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

    glm::vec2 delta(cpos - m_cursor.pos);

    bd::Camera &cam{ m_renderer->getCamera() };


    // rotate around camera right
    glm::mat4 const pitch{ glm::rotate(glm::mat4{ 1.0f },
                                       glm::radians(-delta.y),
                                       cam.getRight()) };


    // Concat pitch with rotation around camera up vector
    glm::mat4 const rotation{ glm::rotate(pitch,
                                          glm::radians(-delta.x),
                                          cam.getUp()) };


    // Give the camera new eye and up
    cam.setEye(glm::vec3{ rotation * glm::vec4{ cam.getEye(), 1 }});
    cam.setUp(glm::vec3{ rotation * glm::vec4{ cam.getUp(), 1 }});

    m_renderer->setViewMatrix(cam.createViewMatrix());
    glm::vec3 f{ glm::normalize(cam.getLookAt() - cam.getEye()) };
    std::cout << "\rView dir: "
              << f.x << ", " << f.y << ", " << f.z
              << std::flush;

  }

  m_cursor.pos = cpos;

}


void
Controls::keyboard_callback(int key, int scancode, int action, int mods)
{
  // on key press
  if (action == GLFW_PRESS) {

    switch (key) {

//      case GLFW_KEY_0:
//        setCameraPosPreset(0);
//        break;
//
//      case GLFW_KEY_1:
//        setCameraPosPreset(1);
//        break;
//
//      case GLFW_KEY_2:
//        setCameraPosPreset(2);
//        break;
//
//      case GLFW_KEY_3:
//        setCameraPosPreset(3);
//        break;

//      case GLFW_KEY_W:
//        g_toggleWireFrame = !g_toggleWireFrame;
//        break;


      case GLFW_KEY_B:
        m_showBlockBoxes = !m_showBlockBoxes;
        std::cout << "Show bounding boxes: " << m_showBlockBoxes << '\n';
        m_renderer->setDrawNonEmptyBoundingBoxes(m_showBlockBoxes);
        break;

      case GLFW_KEY_L:
        m_shouldUseLighting = !m_shouldUseLighting;
        m_renderer->setShouldUseLighting(m_shouldUseLighting);
        std::cout << "Use lighting: " << m_shouldUseLighting << '\n';
        break;

      case GLFW_KEY_Q:
        m_currentBackgroundColor ^= 1;
        std::cout << "Background color: "
                  << ( m_currentBackgroundColor == 0 ? "Dark" : "Light" )
                  << '\n';
        m_renderer->setBackgroundColor(g_backgroundColors[m_currentBackgroundColor]);
        break;

      case GLFW_KEY_T:
        if (mods & GLFW_MOD_SHIFT) {
          m_renderer->setColorMapTexture(ColorMapManager::getPrevMap().getTexture());
          std::cout << "\nColormap: " << ColorMapManager::getCurrentMapName() << '\n';
        } else if (mods & GLFW_MOD_ALT) {
          std::cout << "\n Current map: \n\t Scaling value: "
                    << m_scaleValue
                    << "\n\t"
                    << ColorMapManager::getMapByName(
                        ColorMapManager::getCurrentMapName()).to_string() << std::endl;
        } else {
          m_renderer->setColorMapTexture(ColorMapManager::getNextMap().getTexture());
          std::cout << "\nColormap: " << ColorMapManager::getCurrentMapName() << '\n';
        }

      default:
        break;

    } // switch
  }

  // while holding key down.
  if (action != GLFW_RELEASE) {

    switch (key) {

      // Positive transfer function scaling
      case GLFW_KEY_PERIOD:
        if (mods & GLFW_MOD_SHIFT) {
          m_scaleValue += 0.1f;
        } else if (mods & GLFW_MOD_CONTROL) {
          m_scaleValue += 0.001f;
        } else if (mods & GLFW_MOD_ALT) {
          m_scaleValue += 0.0001f;
        } else {
          m_scaleValue += 0.01f;
        }

        m_renderer->setColorMapScaleValue(m_scaleValue);
        std::cout << "Transfer function scaler: " << m_scaleValue << std::endl;
        break;


        // Negative transfer function scaling
      case GLFW_KEY_COMMA:
        if (mods & GLFW_MOD_SHIFT) {
          m_scaleValue -= 0.1f;
        } else if (mods & GLFW_MOD_CONTROL) {
          m_scaleValue -= 0.001f;
        } else if (mods & GLFW_MOD_ALT) {
          m_scaleValue -= 0.0001f;
        } else {
          m_scaleValue -= 0.01f;
        }

        m_renderer->setColorMapScaleValue(m_scaleValue);
        std::cout << "Transfer function scaler: " << m_scaleValue << std::endl;
        break;


        // n_shiney
      case GLFW_KEY_N:
        if (mods & GLFW_MOD_SHIFT) {
          m_nShiney -= 0.01f;
        } else {
          m_nShiney += 0.01f;
        }

        m_renderer->setShaderNShiney(m_nShiney);
        std::cout << "N shiney: " << m_nShiney << std::endl;
        break;

      case GLFW_KEY_UP:
        if (mods & GLFW_MOD_SHIFT) {
          m_LightVector.z += 0.1f;
        } else {
          m_LightVector.y += 0.1f;
        }

        m_renderer->setShaderLightPos(glm::normalize(m_LightVector));
        std::cout << "Light position: " << m_LightVector.x << ", " << m_LightVector.y
                  << ", "
                  << m_LightVector.z << '\n';
        break;

      case GLFW_KEY_LEFT:
        m_LightVector.x += 0.1f;
        m_renderer->setShaderLightPos(glm::normalize(m_LightVector));
        std::cout << "Light position: " << m_LightVector.x << ", " << m_LightVector.y
                  << ", "
                  << m_LightVector.z << '\n';
        break;

      case GLFW_KEY_RIGHT:
        m_LightVector.x -= 0.1f;
        m_renderer->setShaderLightPos(glm::normalize(m_LightVector));
        std::cout << "Light position: " << m_LightVector.x << ", " << m_LightVector.y
                  << ", "
                  << m_LightVector.z << '\n';
        break;

      case GLFW_KEY_DOWN:
        if (mods & GLFW_MOD_SHIFT) {
          m_LightVector.z -= 0.1f;
        } else {
          m_LightVector.y -= 0.1f;
        }

        m_renderer->setShaderLightPos(glm::normalize(m_LightVector));
        std::cout << "Light position: " << m_LightVector.x << ", " << m_LightVector.y
                  << ", "
                  << m_LightVector.z << '\n';
        break;

      default:
        break;
    } // switch
  }

}


void
Controls::window_size_callback(int width, int height)
{
  m_renderer->resize(width, height);
}


void
Controls::scrollwheel_callback(double xoff, double yoff)
{
  std::cout << "xoff: " << xoff << ", yoff: " << yoff << std::endl;
  // 1.75 scales the vertical motion of the scroll wheel so changing the
  // field of view isn't so slow.
  float fov = static_cast<float>(m_renderer->getFov() + ( yoff * 1.75 ));

  if (fov < 1.0f || fov > 120.0f) {
    return;
  }

  std::cout << "\rfov: " << fov << std::flush;
  m_renderer->setFov(fov);
}


void
Controls::mousebutton_callback(GLFWwindow *window, int button, int action, int mods)
{
//  m_renderer->setIsRotating(
//      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}


Cursor &
Controls::getCursor()
{
  return m_cursor;
}


} // namespace subvol
