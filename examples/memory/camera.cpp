#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

/* Atualiza a matriz de projeção, utilizar o tamanho da tela e o ângulo da
câmera selecionada*/
void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix =
      glm::perspective(glm::radians(m_angle_Current), aspect, 0.1f, 5.0f);
}

// Atualiza a matriz de visão baseado na câmera selecionada
void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eyeAt_Current, m_at_Current, m_up_Current);
}

/* Cria as duas opções de câmeras e atualiza a câmera atual para câmera "Visão
Lateral" */
void Camera::onCreate() {
  m_cameras[0] =
      CameraPosition{"Visão Lateral", glm::vec3(-0.06617f, 0.588165f, 2.19079f),
                     glm::vec3(-0.06617f, 0.588165f, 0.690789f),
                     glm::vec3(0.0f, 1.0f, 0.0f), 60.0f};

  m_cameras[1] = CameraPosition{
      "Vista de Cima", glm::vec3(-0.0843976f, 1.1297f, 0.992652f),
      glm::vec3(-0.0840492f, 0.108946f, -0.106467f),
      glm::vec3(-0.0175263f, 0.732629f, -0.680398f), 90.0f};

  m_eyeAt_Current = m_cameras[0].m_eye;
  m_at_Current = m_cameras[0].m_at;
  m_up_Current = m_cameras[0].m_up;
  m_angle_Current = m_cameras[0].m_angle;

  computeViewMatrix();
  computeProjectionMatrix(sizeT);
}

// Atualiza a câmera atual para câmera selecionada
void Camera::onUpdate(int camSelect) {
  m_eyeAt_Current = m_cameras[camSelect].m_eye;
  m_at_Current = m_cameras[camSelect].m_at;
  m_up_Current = m_cameras[camSelect].m_up;
  m_angle_Current = m_cameras[camSelect].m_angle;

  computeViewMatrix();
  computeProjectionMatrix(sizeT);
}