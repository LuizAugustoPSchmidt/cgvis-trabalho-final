#include "opengl_utils.h"
#include "matrices.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "MeshData.h"
#include <stb_image.h>
#include <tiny_obj_loader.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <set>
#include <algorithm>

// Shader loading functions
void LoadShader(const char *filename, GLuint shader_id) {
  std::ifstream file;
  try {
    file.exceptions(std::ifstream::failbit);
    file.open(filename);
  } catch (std::exception &e) {
    fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
    std::exit(EXIT_FAILURE);
  }
  std::stringstream shader;
  shader << file.rdbuf();
  std::string str = shader.str();
  const GLchar *shader_string = str.c_str();
  const GLint shader_string_length = static_cast<GLint>(str.length());

  glShaderSource(shader_id, 1, &shader_string, &shader_string_length);
  glCompileShader(shader_id);

  GLint compiled_ok;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

  GLint log_length = 0;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

  GLchar *log = new GLchar[log_length];
  glGetShaderInfoLog(shader_id, log_length, &log_length, log);

  if (log_length != 0) {
    std::string output;
    if (!compiled_ok) {
      output += "ERROR: OpenGL compilation of \"";
      output += filename;
      output += "\" failed.\n";
      output += "== Start of compilation log\n";
      output += log;
      output += "== End of compilation log\n";
    } else {
      output += "WARNING: OpenGL compilation of \"";
      output += filename;
      output += "\".\n";
      output += "== Start of compilation log\n";
      output += log;
      output += "== End of compilation log\n";
    }
    fprintf(stderr, "%s", output.c_str());
  }
  delete[] log;
}

GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id) {
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);
  glLinkProgram(program_id);

  GLint linked_ok = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

  if (linked_ok == GL_FALSE) {
    GLint log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
    GLchar *log = new GLchar[log_length];
    glGetProgramInfoLog(program_id, log_length, &log_length, log);
    std::string output;
    output += "ERROR: OpenGL linking of program failed.\n";
    output += "== Start of link log\n";
    output += log;
    output += "\n== End of link log\n";
    delete[] log;
    fprintf(stderr, "%s", output.c_str());
  }
  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);
  return program_id;
}

void ComputeNormals(ObjModel *model) {
  if (!model->attrib.normals.empty())
    return;

  std::set<unsigned int> sgroup_ids;
  for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
    size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();
    for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
      unsigned int sgroup = model->shapes[shape].mesh.smoothing_group_ids[triangle];
      sgroup_ids.insert(sgroup);
    }
  }

  size_t num_vertices = model->attrib.vertices.size() / 3;
  model->attrib.normals.reserve(3 * num_vertices);

  for (const unsigned int &sgroup : sgroup_ids) {
    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
      size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();
      for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
        unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];
        if (sgroup_tri != sgroup) continue;

        glm::vec4 vertices[3];
        for (size_t vertex = 0; vertex < 3; ++vertex) {
          tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
          const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
          const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
          const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
          vertices[vertex] = glm::vec4(vx, vy, vz, 1.0);
        }
        const glm::vec4 n = crossproduct(vertices[1] - vertices[0], vertices[2] - vertices[0]);
        for (size_t vertex = 0; vertex < 3; ++vertex) {
          tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
          num_triangles_per_vertex[idx.vertex_index] += 1;
          vertex_normals[idx.vertex_index] += n;
        }
      }
    }

    std::vector<size_t> normal_indices(num_vertices, 0);
    for (size_t vertex_index = 0; vertex_index < vertex_normals.size(); ++vertex_index) {
      if (num_triangles_per_vertex[vertex_index] == 0) continue;
      glm::vec4 n = vertex_normals[vertex_index] / (float)num_triangles_per_vertex[vertex_index];
      n /= norm(n);
      model->attrib.normals.push_back(n.x);
      model->attrib.normals.push_back(n.y);
      model->attrib.normals.push_back(n.z);
      normal_indices[vertex_index] = (model->attrib.normals.size() / 3) - 1;
    }

    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
      size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();
      for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
        unsigned int sgroup_tri = model->shapes[shape].mesh.smoothing_group_ids[triangle];
        if (sgroup_tri != sgroup) continue;
        for (size_t vertex = 0; vertex < 3; ++vertex) {
          tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
          model->shapes[shape].mesh.indices[3 * triangle + vertex].normal_index = normal_indices[idx.vertex_index];
        }
      }
    }
  }
}

void ExtractMeshData(ObjModel* model, MeshData& out_data) {
    for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
        MeshData::ShapeInfo info;
        info.name = model->shapes[shape].name;
        info.first_index = out_data.indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();
        info.bbox_min = glm::vec3(std::numeric_limits<float>::max());
        info.bbox_max = glm::vec3(std::numeric_limits<float>::lowest());

        for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
            for (size_t vertex = 0; vertex < 3; ++vertex) {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
                out_data.indices.push_back(static_cast<unsigned int>(info.first_index + 3 * triangle + vertex));
                
                const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
                out_data.model_coefficients.push_back(vx);
                out_data.model_coefficients.push_back(vy);
                out_data.model_coefficients.push_back(vz);
                out_data.model_coefficients.push_back(1.0f);

                info.bbox_min.x = std::min(info.bbox_min.x, vx);
                info.bbox_min.y = std::min(info.bbox_min.y, vy);
                info.bbox_min.z = std::min(info.bbox_min.z, vz);
                info.bbox_max.x = std::max(info.bbox_max.x, vx);
                info.bbox_max.y = std::max(info.bbox_max.y, vy);
                info.bbox_max.z = std::max(info.bbox_max.z, vz);

                if (idx.normal_index != -1) {
                    out_data.normal_coefficients.push_back(model->attrib.normals[3 * idx.normal_index + 0]);
                    out_data.normal_coefficients.push_back(model->attrib.normals[3 * idx.normal_index + 1]);
                    out_data.normal_coefficients.push_back(model->attrib.normals[3 * idx.normal_index + 2]);
                    out_data.normal_coefficients.push_back(0.0f);
                }
                if (idx.texcoord_index != -1) {
                    out_data.texture_coefficients.push_back(model->attrib.texcoords[2 * idx.texcoord_index + 0]);
                    out_data.texture_coefficients.push_back(model->attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
            }
        }
        info.num_indices = out_data.indices.size() - info.first_index;
        out_data.shapes.push_back(info);
    }
}

void BuildTrianglesAndAddToVirtualScene(ObjModel *model, std::map<std::string, SceneObject>& scene, const std::string& prefix) {
    MeshData mesh;
    ExtractMeshData(model, mesh);

    GLuint vao_id;
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    auto upload_vbo = [](GLuint location, GLint size, const std::vector<float>& data) {
        if (data.empty()) return;
        GLuint vbo_id;
        glGenBuffers(1, &vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
    };

    upload_vbo(0, 4, mesh.model_coefficients);
    upload_vbo(1, 4, mesh.normal_coefficients);
    upload_vbo(2, 2, mesh.texture_coefficients);

    GLuint ebo_id;
    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    for (const auto& shape : mesh.shapes) {
        SceneObject theobject;
        std::string final_name = prefix + shape.name;
        theobject.name = final_name;
        theobject.first_index = shape.first_index;
        theobject.num_indices = shape.num_indices;
        theobject.rendering_mode = GL_TRIANGLES;
        theobject.vertex_array_object_id = vao_id;
        theobject.bbox_min = shape.bbox_min;
        theobject.bbox_max = shape.bbox_max;
        scene[final_name] = theobject;
    }
}

// Drawing functions
void DrawVirtualObject(const char *object_name, const std::map<std::string, SceneObject>& scene, GLint bbox_min_uniform, GLint bbox_max_uniform) {
  auto it = scene.find(object_name);
  if (it == scene.end()) return;

  const SceneObject& obj = it->second;
  glBindVertexArray(obj.vertex_array_object_id);
  glUniform4f(bbox_min_uniform, obj.bbox_min.x, obj.bbox_min.y, obj.bbox_min.z, 1.0f);
  glUniform4f(bbox_max_uniform, obj.bbox_max.x, obj.bbox_max.y, obj.bbox_max.z, 1.0f);
  glDrawElements(obj.rendering_mode, obj.num_indices, GL_UNSIGNED_INT, (void *)(obj.first_index * sizeof(GLuint)));
  glBindVertexArray(0);
}

void PrintObjModelInfo(ObjModel *model) {
  const tinyobj::attrib_t &attrib = model->attrib;
  const std::vector<tinyobj::shape_t> &shapes = model->shapes;
  const std::vector<tinyobj::material_t> &materials = model->materials;
  printf("# of vertices  : %d\n# of normals   : %d\n# of texcoords : %d\n# of shapes    : %d\n# of materials : %d\n", (int)(attrib.vertices.size() / 3), (int)(attrib.normals.size() / 3), (int)(attrib.texcoords.size() / 2), (int)shapes.size(), (int)materials.size());
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\nSize of shape[%ld].indices: %lu\nshape[%ld].num_faces: %lu\n", static_cast<long>(i), shapes[i].name.c_str(), static_cast<long>(i), static_cast<unsigned long>(shapes[i].mesh.indices.size()), static_cast<long>(i), static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));
  }
}
