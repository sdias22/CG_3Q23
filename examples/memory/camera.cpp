#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix =
      glm::perspective(glm::radians(m_angle_Current), aspect, 0.1f, 5.0f);
}

void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eyeAt_Current, m_at_Current, m_up_Current);
}

void Camera::onCreate() {
  m_cameras[0] = CameraPosition{"Vista de Cima", glm::vec3(0.0f, 1.0f, 0.0005f),
                                glm::vec3(0.0f, -1.0f, 0.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f), 90.0f};

  m_cameras[1] =
      CameraPosition{"CAM1", glm::vec3(-0.06617f, 0.588165f, 2.19079f),
                     glm::vec3(-0.06617f, 0.588165f, 0.690789f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};

  m_cameras[2] =
      CameraPosition{"CAM2", glm::vec3(2.2081f, 0.591345f, -0.0981149f),
                     glm::vec3(0.708111f, 0.591345f, -0.105421f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};

  m_cameras[3] =
      CameraPosition{"CAM3", glm::vec3(-0.110116f, 0.554741f, -2.32302f),
                     glm::vec3(-0.0911373f, 0.554741f, -0.823134f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};

  m_cameras[4] =
      CameraPosition{"CAM4", glm::vec3(-2.31217f, 0.554741f, -0.0896668f),
                     glm::vec3(-0.812174f, 0.554741f, -0.0918008f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};

  m_cameras[5] =
      CameraPosition{"CAM5", glm::vec3(-1.49516f, 0.438277f, 1.49457f),
                     glm::vec3(-0.499002f, 0.438277f, 0.373111f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};
  m_cameras[6] =
      CameraPosition{"CAM6", glm::vec3(1.30522f, 0.379888f, -1.65208f),
                     glm::vec3(0.300989f, 0.379888f, -0.53785f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};

  m_eyeAt_Current = m_cameras[0].m_eye;
  m_at_Current = m_cameras[0].m_at;
  m_up_Current = m_cameras[0].m_up;
  m_angle_Current = m_cameras[0].m_angle;

  computeViewMatrix();
  computeProjectionMatrix(sizeT);
}

void Camera::onUpdate(int camSelect) {
  m_eyeAt_Current = m_cameras[camSelect].m_eye;
  m_at_Current = m_cameras[camSelect].m_at;
  m_up_Current = m_cameras[camSelect].m_up;
  m_angle_Current = m_cameras[camSelect].m_angle;

  computeViewMatrix();
  computeProjectionMatrix(sizeT);
}