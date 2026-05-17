#ifndef SCENE_H
#define SCENE_H

// clang-format off
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <tiny_obj_loader.h>
// clang-format on
//
// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj".
struct ObjModel {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  ObjModel(
      const char *filename, const char *basepath = NULL, bool triangulate = true
  );
};

// Estrutura que armazenará dados necessários para renderizar cada objeto da
// cena virtual.
struct SceneObject {
  std::string name;
  size_t first_index;
  size_t num_indices;
  GLenum rendering_mode;
  GLuint vertex_array_object_id;
  glm::vec3 bbox_min;
  glm::vec3 bbox_max;
};

// Variáveis globais (declaradas como extern para serem acessíveis em outros
// arquivos)
extern std::map<std::string, SceneObject> g_VirtualScene;
extern std::stack<glm::mat4> g_MatrixStack;
extern float g_ScreenRatio;
extern float g_AngleX;
extern float g_AngleY;
extern float g_AngleZ;
extern bool g_LeftMouseButtonPressed;
extern bool g_RightMouseButtonPressed;
extern bool g_MiddleMouseButtonPressed;
extern float g_CameraTheta;
extern float g_CameraPhi;
extern float g_CameraDistance;
extern float g_ForearmAngleZ;
extern float g_ForearmAngleX;
extern float g_TorsoPositionX;
extern float g_TorsoPositionY;
extern bool g_UsePerspectiveProjection;
extern bool g_ShowInfoText;
extern GLuint g_GpuProgramID;
extern GLint g_model_uniform;
extern GLint g_view_uniform;
extern GLint g_projection_uniform;
extern GLint g_object_id_uniform;
extern GLint g_bbox_min_uniform;
extern GLint g_bbox_max_uniform;
extern GLuint g_NumLoadedTextures;

// Funções compartilhadas
void BuildTrianglesAndAddToVirtualScene(ObjModel *);
void ComputeNormals(ObjModel *model);
void LoadShadersFromFiles();
void LoadTextureImage(const char *filename);
void DrawVirtualObject(const char *object_name);
GLuint LoadShader_Vertex(const char *filename);
GLuint LoadShader_Fragment(const char *filename);
void LoadShader(const char *filename, GLuint shader_id);
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow *window);
float TextRendering_CharWidth(GLFWwindow *window);
void TextRendering_PrintString(
    GLFWwindow *window,
    const std::string &str,
    float x,
    float y,
    float scale = 1.0f
);
void TextRendering_PrintMatrix(
    GLFWwindow *window, glm::mat4 M, float x, float y, float scale = 1.0f
);
void TextRendering_PrintVector(
    GLFWwindow *window, glm::vec4 v, float x, float y, float scale = 1.0f
);
void TextRendering_PrintMatrixVectorProduct(
    GLFWwindow *window,
    glm::mat4 M,
    glm::vec4 v,
    float x,
    float y,
    float scale = 1.0f
);
void TextRendering_PrintMatrixVectorProductMoreDigits(
    GLFWwindow *window,
    glm::mat4 M,
    glm::vec4 v,
    float x,
    float y,
    float scale = 1.0f
);
void TextRendering_PrintMatrixVectorProductDivW(
    GLFWwindow *window,
    glm::mat4 M,
    glm::vec4 v,
    float x,
    float y,
    float scale = 1.0f
);

void TextRendering_ShowEulerAngles(GLFWwindow *window);
void TextRendering_ShowProjection(GLFWwindow *window);
void TextRendering_ShowFramesPerSecond(GLFWwindow *window);

#endif // SCENE_H
