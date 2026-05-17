#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include "scene.h"

// Shader loading functions
GLuint LoadShader_Vertex(const char *filename);
GLuint LoadShader_Fragment(const char *filename);
void LoadShader(const char *filename, GLuint shader_id);
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id);
void LoadShadersFromFiles();

// Asset loading functions
void LoadTextureImage(const char *filename);
void BuildTrianglesAndAddToVirtualScene(ObjModel *model);
void ComputeNormals(ObjModel *model);

// Drawing functions
void DrawVirtualObject(const char *object_name);

// Callbacks
void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

// Matrix stack functions
void PushMatrix(glm::mat4 M);
void PopMatrix(glm::mat4 &M);

// Text rendering and display functions
void TextRendering_ShowModelViewProjection(
    GLFWwindow *window,
    glm::mat4 projection,
    glm::mat4 view,
    glm::mat4 model,
    glm::vec4 p_model
);
void TextRendering_ShowEulerAngles(GLFWwindow *window);
void TextRendering_ShowProjection(GLFWwindow *window);
void TextRendering_ShowFramesPerSecond(GLFWwindow *window);

// Debugging functions
void PrintObjModelInfo(ObjModel *model);

#endif // OPENGL_UTILS_H
