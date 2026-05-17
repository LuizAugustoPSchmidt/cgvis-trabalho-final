//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Computação Gráfica e Visualização I
//               Prof. Eduardo Gastal
//
//     CÓDIGO BASE PARA O TRABALHO FINAL
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
// clang-format off
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <set>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"
#include "scene.h"
#include "Application.h"
#include "opengl_utils.h"
// clang-format on

// Definições das variáveis globais
std::map<std::string, SceneObject> g_VirtualScene;
std::stack<glm::mat4> g_MatrixStack;
float g_ScreenRatio = 1.0f;
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false;
bool g_MiddleMouseButtonPressed = false;
float g_CameraTheta = 0.0f;
float g_CameraPhi = 0.0f;
float g_CameraDistance = 3.5f;
float g_ForearmAngleZ = 0.0f;
float g_ForearmAngleX = 0.0f;
float g_TorsoPositionX = 0.0f;
float g_TorsoPositionY = 0.0f;
bool g_UsePerspectiveProjection = true;
bool g_ShowInfoText = true;
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;
GLuint g_NumLoadedTextures = 0;

// Implementação do construtor de ObjModel
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

  std::string warn;
  std::string err;
  bool ret = tinyobj::LoadObj(
      &attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate
  );

  if (!err.empty())
    fprintf(stderr, "\n%s\n", err.c_str());

  if (!ret)
    throw std::runtime_error("Erro ao carregar modelo.");

  for (size_t shape = 0; shape < shapes.size(); ++shape) {
    if (shapes[shape].name.empty()) {
      fprintf(
          stderr,
          "*********************************************\n"
          "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
          "Veja "
          "https://www.inf.ufrgs.br/~eslgastal/"
          "fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
          "*********************************************\n",
          filename
      );
      throw std::runtime_error("Objeto sem nome.");
    }
    printf("- Objeto '%s'\n", shapes[shape].name.c_str());
  }
  printf("OK.\n");
}

int main(int argc, char *argv[]) {
  Application app;
  if (!app.Init()) {
    return EXIT_FAILURE;
  }

  app.LoadAssets(argc, argv);
  app.Run();

  return EXIT_SUCCESS;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
    g_LeftMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    g_LeftMouseButtonPressed = false;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
    g_RightMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    g_RightMouseButtonPressed = false;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
    g_MiddleMouseButtonPressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
    g_MiddleMouseButtonPressed = false;
  }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  if (g_LeftMouseButtonPressed) {
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;
    g_CameraTheta -= 0.01f * dx;
    g_CameraPhi += 0.01f * dy;
    float phimax = 3.141592f / 2;
    float phimin = -phimax;
    if (g_CameraPhi > phimax) g_CameraPhi = phimax;
    if (g_CameraPhi < phimin) g_CameraPhi = phimin;
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
  }
  if (g_RightMouseButtonPressed) {
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;
    g_ForearmAngleZ -= 0.01f * dx;
    g_ForearmAngleX += 0.01f * dy;
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
  }
  if (g_MiddleMouseButtonPressed) {
    float dx = xpos - g_LastCursorPosX;
    float dy = ypos - g_LastCursorPosY;
    g_TorsoPositionX += 0.01f * dx;
    g_TorsoPositionY -= 0.01f * dy;
    g_LastCursorPosX = xpos;
    g_LastCursorPosY = ypos;
  }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  g_CameraDistance -= 0.1f * yoffset;
  const float verysmallnumber = std::numeric_limits<float>::epsilon();
  if (g_CameraDistance < verysmallnumber) g_CameraDistance = verysmallnumber;
}

void Correcao_KeyCallback(int key, int action, int mod);

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado.
void KeyCallback(
    GLFWwindow *window, int key, int scancode, int action, int mod
) {
  Correcao_KeyCallback(key, action, mod);
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  float delta = 3.141592 / 16;
  if (key == GLFW_KEY_X && action == GLFW_PRESS) g_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  if (key == GLFW_KEY_Y && action == GLFW_PRESS) g_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) g_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;

  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    g_AngleX = 0.0f; g_AngleY = 0.0f; g_AngleZ = 0.0f;
    g_ForearmAngleX = 0.0f; g_ForearmAngleZ = 0.0f;
    g_TorsoPositionX = 0.0f; g_TorsoPositionY = 0.0f;
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS) g_UsePerspectiveProjection = true;
  if (key == GLFW_KEY_O && action == GLFW_PRESS) g_UsePerspectiveProjection = false;
  if (key == GLFW_KEY_H && action == GLFW_PRESS) g_ShowInfoText = !g_ShowInfoText;
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    LoadShadersFromFiles();
    fprintf(stdout, "Shaders recarregados!\n");
    fflush(stdout);
  }
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char *description) {
  fprintf(stderr, "ERROR: GLFW: %s\n", description);
}
