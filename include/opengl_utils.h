#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <map>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "scene.h"

struct MeshData;

// Shader loading functions
void LoadShader(const char *filename, GLuint shader_id);
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);

// Asset loading functions
void ExtractMeshData(ObjModel* model, MeshData& out_data);
void BuildTrianglesAndAddToVirtualScene(ObjModel *model, std::map<std::string, SceneObject>& scene);
void ComputeNormals(ObjModel *model);

// Drawing functions
void DrawVirtualObject(const char *object_name, const std::map<std::string, SceneObject>& scene, GLint bbox_min_uniform, GLint bbox_max_uniform);

// Text rendering and display functions
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow *window);
float TextRendering_CharWidth(GLFWwindow *window);
void TextRendering_PrintString(
    GLFWwindow *window,
    const std::string &str,
    float x,
    float y,
    float scale = 1.0f,
    glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
);
void TextRendering_DrawRectangle(
    GLFWwindow *window,
    float x,
    float y,
    float width,
    float height,
    glm::vec4 color
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

// Debugging functions
void PrintObjModelInfo(ObjModel *model);

#endif // OPENGL_UTILS_H
