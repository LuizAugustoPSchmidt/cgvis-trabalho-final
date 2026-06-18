#include "Application.h"
#include <cstdio>
#include <iostream>
#include "matrices.h"

// External function for automated correction
void Correcao_KeyCallback(int key, int action, int mod);

namespace {
void ErrorCallback(int error, const char *description) {
  fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

void KeyCallback(
    GLFWwindow *window, int key, int scancode, int action, int mod
) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app)
    app->KeyCallback(key, scancode, action, mod);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app)
    app->MouseButtonCallback(button, action, mods);
}

void CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app)
    app->CursorPosCallback(xpos, ypos);
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app)
    app->ScrollCallback(xoffset, yoffset);
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
  if (app)
    app->FramebufferSizeCallback(width, height);
}
} // namespace

void Application::SetCallbacks() {
  glfwSetErrorCallback(ErrorCallback);
  SetKeyCallback();
  SetMouseButtonCallback();
  SetCursorPosCallback();
  SetMouseScrollCallback();
  SetFramebufferSizeCallback();
}

void Application::SetKeyCallback() {
  glfwSetKeyCallback(m_Window, ::KeyCallback);
}

void Application::SetMouseButtonCallback() {
  glfwSetMouseButtonCallback(m_Window, ::MouseButtonCallback);
}

void Application::SetCursorPosCallback() {
  glfwSetCursorPosCallback(m_Window, ::CursorPosCallback);
}

void Application::SetMouseScrollCallback() {
  glfwSetScrollCallback(m_Window, ::ScrollCallback);
}

void Application::SetFramebufferSizeCallback() {
  glfwSetFramebufferSizeCallback(m_Window, ::FramebufferSizeCallback);
}

// Callback Implementations
void Application::KeyCallback(int key, int scancode, int action, int mod) {
  Correcao_KeyCallback(key, action, mod);

  if (m_StartScreen) {
    if ((key == GLFW_KEY_SPACE || key == GLFW_KEY_ENTER) && action == GLFW_PRESS)
      m_StartScreen = false;
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
      glfwSetWindowShouldClose(m_Window, GL_TRUE);
    return;
  }

  if (m_Paused) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      m_Paused = false;
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
      Reset();
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
      glfwSetWindowShouldClose(m_Window, GL_TRUE);
    return;
  }

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !m_GameOver && !m_Victory)
    m_Paused = true;
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(m_Window, GL_TRUE);

  if (key == GLFW_KEY_C && action == GLFW_PRESS)
    m_CameraMode = (m_CameraMode == CameraMode::ThirdPerson)
                       ? CameraMode::FirstPerson
                       : CameraMode::ThirdPerson;
  if (key == GLFW_KEY_W) {
    if (action == GLFW_PRESS)
      m_Player->SetBoosting(true);
    else if (action == GLFW_RELEASE)
      m_Player->SetBoosting(false);
  }
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && !m_Paused && !m_GameOver)
    m_Player->Shoot();
  if (key == GLFW_KEY_I && action == GLFW_PRESS) {
    m_InvertY = !m_InvertY;
#if !RELEASE
    printf(
        "Mouse Y Inversion: %s\n",
        m_InvertY ? "ON (Flight)" : "OFF (Normal)"
    );
#endif
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    m_Paused = !m_Paused;
    printf("Game %s\n", m_Paused ? "PAUSED" : "RESUMED");
  }
  if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    m_VsyncEnabled = !m_VsyncEnabled;
    glfwSwapInterval(m_VsyncEnabled ? 1 : 0);
    printf("V-Sync: %s\n", m_VsyncEnabled ? "ON" : "OFF");
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    // Shader reload logic could be moved here or kept in opengl_utils
    // For now, let's just re-instantiate our Shader object
    m_MainShader = std::make_unique<Shader>(
        "../../src/shader_vertex.glsl",
        "../../src/shader_fragment.glsl"
    );
    fprintf(stdout, "Shaders recarregados!\n");
    fflush(stdout);
  }
}

void Application::MouseButtonCallback(int button, int action, int mods) {
  if (m_StartScreen) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
      m_StartScreen = false;
    return;
  }

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    glfwGetCursorPos(m_Window, &m_LastCursorPosX, &m_LastCursorPosY);
    m_LeftMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    m_LeftMouseButtonPressed = false;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    glfwGetCursorPos(m_Window, &m_LastCursorPosX, &m_LastCursorPosY);
    m_RightMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    m_RightMouseButtonPressed = false;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    glfwGetCursorPos(m_Window, &m_LastCursorPosX, &m_LastCursorPosY);
    m_MiddleMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    m_MiddleMouseButtonPressed = false;
  }
}

void Application::CursorPosCallback(double xpos, double ypos) {
  if (m_LeftMouseButtonPressed) {
    float dx = xpos - m_LastCursorPosX;
    float dy = ypos - m_LastCursorPosY;

    m_CameraTheta -= 0.01f * dx;

    m_CameraPhi += 0.01f * (m_InvertY ? dy : -dy);

    float phimax = 3.141592f / 2;
    float phimin = -phimax;

    if (m_CameraPhi > phimax)
      m_CameraPhi = phimax;
    if (m_CameraPhi < phimin)
      m_CameraPhi = phimin;

    m_LastCursorPosX = xpos;
    m_LastCursorPosY = ypos;
  }
  if (m_RightMouseButtonPressed) {
    float dx = xpos - m_LastCursorPosX;
    float dy = ypos - m_LastCursorPosY;
    m_ForearmAngleZ -= 0.01f * dx;
    m_ForearmAngleX += 0.01f * dy;
    m_LastCursorPosX = xpos;
    m_LastCursorPosY = ypos;
  }
  if (m_MiddleMouseButtonPressed) {
    float dx = xpos - m_LastCursorPosX;
    float dy = ypos - m_LastCursorPosY;
    m_TorsoPositionX += 0.01f * dx;
    m_TorsoPositionY -= 0.01f * dy;
    m_LastCursorPosX = xpos;
    m_LastCursorPosY = ypos;
  }
}

void Application::ScrollCallback(double xoffset, double yoffset) {
#if !RELEASE
  m_DebugZoom += (float)yoffset * 0.1f;
  if (m_DebugZoom < 0.6f)
    m_DebugZoom = 0.6f;
  printf("Debug Zoom: %.1f\n", m_DebugZoom);
#endif
}

void Application::FramebufferSizeCallback(int width, int height) {
  glViewport(0, 0, width, height);
  m_ScreenRatio = (float)width / height;
}
