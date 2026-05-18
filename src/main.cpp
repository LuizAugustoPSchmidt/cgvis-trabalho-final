//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Computação Gráfica e Visualização I
//               Prof. Eduardo Gastal
//
//     CÓDIGO BASE PARA O TRABALHO FINAL
//

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include "Application.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "scene.h"
#include "utils.h"

// ObjModel implementation
ObjModel::ObjModel(
    const char *filename, const char *basepath, bool triangulate
) {
  printf("Carregando objetos do arquivo \"%s\"...\n", filename);
  std::string fullpath(filename);
  std::string dirname;
  if (basepath == NULL) {
    auto i = fullpath.find_last_of("/");
    if (i != std::string::npos) {
      dirname = fullpath.substr(0, i + 1);
      basepath = dirname.c_str();
    }
  }
  std::string warn, err;
  bool ret = tinyobj::LoadObj(
      &attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate
  );
  if (!err.empty())
    fprintf(stderr, "\n%s\n", err.c_str());
  if (!ret)
    throw std::runtime_error("Erro ao carregar modelo.");
  for (size_t shape = 0; shape < shapes.size(); ++shape) {
    if (shapes[shape].name.empty())
      throw std::runtime_error("Objeto sem nome.");
    printf("- Objeto '%s'\n", shapes[shape].name.c_str());
  }
  printf("OK.\n");
}

// Global callback wrappers
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

int main(int argc, char *argv[]) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  Application app;
  if (!app.Init())
    return EXIT_FAILURE;

  // We need to set the callbacks before loading assets if they rely on window
  // state
  glfwSetKeyCallback(glfwGetCurrentContext(), KeyCallback);
  glfwSetMouseButtonCallback(glfwGetCurrentContext(), MouseButtonCallback);
  glfwSetCursorPosCallback(glfwGetCurrentContext(), CursorPosCallback);
  glfwSetScrollCallback(glfwGetCurrentContext(), ScrollCallback);
  glfwSetFramebufferSizeCallback(
      glfwGetCurrentContext(), FramebufferSizeCallback
  );

  app.LoadAssets(argc, argv);
  app.Run();
  return EXIT_SUCCESS;
}
