#ifndef SCENE_H
#define SCENE_H

// clang-format off
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <tiny_obj_loader.h>
// clang-format on

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

#endif // SCENE_H
