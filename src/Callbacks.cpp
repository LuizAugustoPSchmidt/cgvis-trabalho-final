#include "Application.h"
#include <cstdio>

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
