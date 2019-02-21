//
// Created by Jim Pelton on 8/1/16.
//

#ifndef subvol_renderer_h__
#define subvol_renderer_h__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bd
{

class Camera
{
public:
  /// \brief Create a camera at (0,0,0), looking at (0,0,-1) with up vector (0,1,0).
  Camera()
      : Camera{{ 0, 0, 0 },
               { 0, 0, -1 },
               { 0, 1, 0 }}
  {
  }


  Camera(glm::vec3 const &eye, glm::vec3 const &lookAt, glm::vec3 const &up)
      : m_eye{ eye }
      , m_lookAt{ lookAt }
      , m_up{ up }
  {
  }


  ~Camera()
  {
  }


  /// \brief Create right handed view matrix with glm::lookAt().
  glm::mat4
  createViewMatrix() const
  {
    return glm::lookAtRH(m_eye, m_lookAt, m_up);
  };


  glm::vec3 const &
  getEye() const
  {
    return m_eye;
  }


  glm::vec3 const &
  getLookAt() const
  {
    return m_lookAt;
  }


  glm::vec3 const &
  getUp() const
  {
    return m_up;
  }


  glm::vec3
  getRight() const
  {
    glm::vec3 const f = glm::normalize(m_lookAt - m_eye);
    glm::vec3 r = glm::cross(f, m_up);
    return r;
  }


  void
  setEye(glm::vec3 const &v)
  {
    m_eye = v;
  }


  void
  setLookAt(glm::vec3 const &v)
  {
    m_lookAt = v;
  }


  void
  setUp(glm::vec3 const &v)
  {
    m_up = v;
  }


private:
  /// \brief Camera location
  glm::vec3 m_eye;
  /// \brief Camera look at vector
  glm::vec3 m_lookAt;
  /// \brief Camera up vector
  glm::vec3 m_up;

};

class Renderer
{

public:
  Renderer();


  virtual ~Renderer();

  virtual void
  draw() = 0;

  virtual bool
  initialize() = 0;

  void
  resize(unsigned int w, unsigned int h);


  unsigned int
  getViewPortWidth() const;


  unsigned int
  getViewPortHeight() const;

  float
  getAspectRatio() const;

//  void setNearClip(float near);

//  void setFarClip(float far);

  /// \brief Set FOV, expressed in radians.
  /// \note Updates world-view-projection matrix
  void
  setFov(float fov);


  float
  getFov() const;


  float
  getFocalLength() const;

  void
  setClearColor(glm::vec3 const &color);


  glm::vec3 const &
  getClearColor() const;


  void
  setWorldMatrix(glm::mat4 const &world);


  glm::mat4 const &
  getWorldMatrix() const;


  void
  setViewMatrix(glm::mat4 const &view);


  glm::mat4 const &
  getViewMatrix() const;


  void
  setProjectionMatrix(glm::mat4 const &proj);


  glm::mat4 const &
  getProjectionMatrix() const;


  glm::mat4 const &
  getWorldViewProjectionMatrix() const;


  Camera const &
  getCamera() const;


  Camera &
  getCamera();

//  void setShader(Shader *s);
private:
  void
  updateProjectionMatrix();


private:
  /// Window X pos
  unsigned int m_viewPortXPos;
  /// Window Y pos
  unsigned int m_viewPortYPos;
  /// Window width
  unsigned int m_viewPortWidth;
  /// Window height
  unsigned int m_viewPortHeight;
  /// Aspect ratio of the view port
  float m_aspectRatio;
  /// Near clipping plain distance
  float m_near;
  /// Far clipping plain distance
  float m_far;
  /// Field of view
  float m_fov;
  /// focal length
  float m_focalLength;

  /// Clear color
  glm::vec3 m_clearColor;

  /// World transform
  glm::mat4 m_worldMat;
  /// View transform
  glm::mat4 m_viewMat;
  /// Perspective transform
  glm::mat4 m_projMat;
  /// WVP
  glm::mat4 m_wvpMat;

  Camera m_camera;
//  bd::Shader *m_shader;

};


} // namespace subvol

#endif //! subvol_renderer_h__
