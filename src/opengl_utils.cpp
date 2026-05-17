#include "opengl_utils.h"
#include "matrices.h"
#include <stb_image.h>
#include <tiny_obj_loader.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <set>
#include <algorithm>

// Shader loading functions
GLuint LoadShader_Vertex(const char *filename) {
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  LoadShader(filename, vertex_shader_id);
  return vertex_shader_id;
}

GLuint LoadShader_Fragment(const char *filename) {
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  LoadShader(filename, fragment_shader_id);
  return fragment_shader_id;
}

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

void LoadShadersFromFiles() {
  GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
  GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

  if (g_GpuProgramID != 0)
    glDeleteProgram(g_GpuProgramID);

  g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

  g_model_uniform = glGetUniformLocation(g_GpuProgramID, "model");
  g_view_uniform = glGetUniformLocation(g_GpuProgramID, "view");
  g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection");
  g_object_id_uniform = glGetUniformLocation(g_GpuProgramID, "object_id");
  g_bbox_min_uniform = glGetUniformLocation(g_GpuProgramID, "bbox_min");
  g_bbox_max_uniform = glGetUniformLocation(g_GpuProgramID, "bbox_max");

  glUseProgram(g_GpuProgramID);
  glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
  glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
  glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
  glUseProgram(0);
}

// Asset loading functions
void LoadTextureImage(const char *filename) {
  printf("Carregando imagem \"%s\"... ", filename);
  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

  if (data == NULL) {
    fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
    std::exit(EXIT_FAILURE);
  }

  printf("OK (%dx%d).\n", width, height);

  GLuint texture_id, sampler_id;
  glGenTextures(1, &texture_id);
  glGenSamplers(1, &sampler_id);

  glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

  GLuint textureunit = g_NumLoadedTextures;
  glActiveTexture(GL_TEXTURE0 + textureunit);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindSampler(textureunit, sampler_id);

  stbi_image_free(data);
  g_NumLoadedTextures += 1;
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

void BuildTrianglesAndAddToVirtualScene(ObjModel *model) {
  GLuint vertex_array_object_id;
  glGenVertexArrays(1, &vertex_array_object_id);
  glBindVertexArray(vertex_array_object_id);

  std::vector<GLuint> indices;
  std::vector<float> model_coefficients;
  std::vector<float> normal_coefficients;
  std::vector<float> texture_coefficients;

  for (size_t shape = 0; shape < model->shapes.size(); ++shape) {
    size_t first_index = indices.size();
    size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();
    auto bbox_min = glm::vec3(std::numeric_limits<float>::max());
    auto bbox_max = glm::vec3(std::numeric_limits<float>::min());

    for (size_t triangle = 0; triangle < num_triangles; ++triangle) {
      for (size_t vertex = 0; vertex < 3; ++vertex) {
        tinyobj::index_t idx = model->shapes[shape].mesh.indices[3 * triangle + vertex];
        indices.push_back(first_index + 3 * triangle + vertex);
        const float vx = model->attrib.vertices[3 * idx.vertex_index + 0];
        const float vy = model->attrib.vertices[3 * idx.vertex_index + 1];
        const float vz = model->attrib.vertices[3 * idx.vertex_index + 2];
        model_coefficients.push_back(vx);
        model_coefficients.push_back(vy);
        model_coefficients.push_back(vz);
        model_coefficients.push_back(1.0f);
        bbox_min.x = std::min(bbox_min.x, vx);
        bbox_min.y = std::min(bbox_min.y, vy);
        bbox_min.z = std::min(bbox_min.z, vz);
        bbox_max.x = std::max(bbox_max.x, vx);
        bbox_max.y = std::max(bbox_max.y, vy);
        bbox_max.z = std::max(bbox_max.z, vz);

        if (idx.normal_index != -1) {
          normal_coefficients.push_back(model->attrib.normals[3 * idx.normal_index + 0]);
          normal_coefficients.push_back(model->attrib.normals[3 * idx.normal_index + 1]);
          normal_coefficients.push_back(model->attrib.normals[3 * idx.normal_index + 2]);
          normal_coefficients.push_back(0.0f);
        }
        if (idx.texcoord_index != -1) {
          texture_coefficients.push_back(model->attrib.texcoords[2 * idx.texcoord_index + 0]);
          texture_coefficients.push_back(model->attrib.texcoords[2 * idx.texcoord_index + 1]);
        }
      }
    }
    SceneObject theobject;
    theobject.name = model->shapes[shape].name;
    theobject.first_index = first_index;
    theobject.num_indices = indices.size() - first_index;
    theobject.rendering_mode = GL_TRIANGLES;
    theobject.vertex_array_object_id = vertex_array_object_id;
    theobject.bbox_min = bbox_min;
    theobject.bbox_max = bbox_max;
    g_VirtualScene[model->shapes[shape].name] = theobject;
  }

  auto add_vbo = [](GLuint location, GLint size, const std::vector<float>& data) {
    GLuint vbo_id;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  };

  add_vbo(0, 4, model_coefficients);
  if (!normal_coefficients.empty()) add_vbo(1, 4, normal_coefficients);
  if (!texture_coefficients.empty()) add_vbo(2, 2, texture_coefficients);

  GLuint indices_id;
  glGenBuffers(1, &indices_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
  glBindVertexArray(0);
}

// Drawing functions
void DrawVirtualObject(const char *object_name) {
  glBindVertexArray(g_VirtualScene[object_name].vertex_array_object_id);
  glm::vec3 bbox_min = g_VirtualScene[object_name].bbox_min;
  glm::vec3 bbox_max = g_VirtualScene[object_name].bbox_max;
  glUniform4f(g_bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
  glUniform4f(g_bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);
  glDrawElements(g_VirtualScene[object_name].rendering_mode, g_VirtualScene[object_name].num_indices, GL_UNSIGNED_INT, (void *)(g_VirtualScene[object_name].first_index * sizeof(GLuint)));
  glBindVertexArray(0);
}

// Callbacks
void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  g_ScreenRatio = (float)width / height;
}

// Matrix stack functions
void PushMatrix(glm::mat4 M) { g_MatrixStack.push(M); }
void PopMatrix(glm::mat4 &M) {
  if (g_MatrixStack.empty()) M = Matrix_Identity();
  else { M = g_MatrixStack.top(); g_MatrixStack.pop(); }
}

// Text rendering display functions (declarations found in scene.h)
void TextRendering_ShowModelViewProjection(GLFWwindow *window, glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 p_model) {
  if (!g_ShowInfoText) return;
  glm::vec4 p_world = model * p_model;
  glm::vec4 p_camera = view * p_world;
  glm::vec4 p_clip = projection * p_camera;
  glm::vec4 p_ndc = p_clip / p_clip.w;
  float pad = TextRendering_LineHeight(window);
  TextRendering_PrintString(window, " Model matrix             Model     In World Coords.", -1.0f, 1.0f - pad, 1.0f);
  TextRendering_PrintMatrixVectorProduct(window, model, p_model, -1.0f, 1.0f - 2 * pad, 1.0f);
  TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f - 6 * pad, 1.0f);
  TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f - 7 * pad, 1.0f);
  TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f - 8 * pad, 1.0f);
  TextRendering_PrintString(window, " View matrix              World     In Camera Coords.", -1.0f, 1.0f - 9 * pad, 1.0f);
  TextRendering_PrintMatrixVectorProduct(window, view, p_world, -1.0f, 1.0f - 10 * pad, 1.0f);
  TextRendering_PrintString(window, "                                        |  ", -1.0f, 1.0f - 14 * pad, 1.0f);
  TextRendering_PrintString(window, "                            .-----------'  ", -1.0f, 1.0f - 15 * pad, 1.0f);
  TextRendering_PrintString(window, "                            V              ", -1.0f, 1.0f - 16 * pad, 1.0f);
  TextRendering_PrintString(window, " Projection matrix        Camera                    In NDC", -1.0f, 1.0f - 17 * pad, 1.0f);
  TextRendering_PrintMatrixVectorProductDivW(window, projection, p_camera, -1.0f, 1.0f - 18 * pad, 1.0f);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glm::vec2 a = glm::vec2(-1, -1), b = glm::vec2(+1, +1), p = glm::vec2(0, 0), q = glm::vec2(width, height);
  glm::mat4 viewport_mapping = Matrix((q.x - p.x) / (b.x - a.x), 0.0f, 0.0f, (b.x * p.x - a.x * q.x) / (b.x - a.x), 0.0f, (q.y - p.y) / (b.y - a.y), 0.0f, (b.y * p.y - a.y * q.y) / (b.y - a.y), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  TextRendering_PrintString(window, "                                                       |  ", -1.0f, 1.0f - 22 * pad, 1.0f);
  TextRendering_PrintString(window, "                            .--------------------------'  ", -1.0f, 1.0f - 23 * pad, 1.0f);
  TextRendering_PrintString(window, "                            V                           ", -1.0f, 1.0f - 24 * pad, 1.0f);
  TextRendering_PrintString(window, " Viewport matrix           NDC      In Pixel Coords.", -1.0f, 1.0f - 25 * pad, 1.0f);
  TextRendering_PrintMatrixVectorProductMoreDigits(window, viewport_mapping, p_ndc, -1.0f, 1.0f - 26 * pad, 1.0f);
}

void TextRendering_ShowEulerAngles(GLFWwindow *window) {
  if (!g_ShowInfoText) return;
  float pad = TextRendering_LineHeight(window);
  char buffer[80];
  snprintf(buffer, 80, "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n", g_AngleZ, g_AngleY, g_AngleX);
  TextRendering_PrintString(window, buffer, -1.0f + pad / 10, -1.0f + 2 * pad / 10, 1.0f);
}

void TextRendering_ShowProjection(GLFWwindow *window) {
  if (!g_ShowInfoText) return;
  float lineheight = TextRendering_LineHeight(window), charwidth = TextRendering_CharWidth(window);
  if (g_UsePerspectiveProjection) TextRendering_PrintString(window, "Perspective", 1.0f - 13 * charwidth, -1.0f + 2 * lineheight / 10, 1.0f);
  else TextRendering_PrintString(window, "Orthographic", 1.0f - 13 * charwidth, -1.0f + 2 * lineheight / 10, 1.0f);
}

void TextRendering_ShowFramesPerSecond(GLFWwindow *window) {
  if (!g_ShowInfoText) return;
  static float old_seconds = (float)glfwGetTime();
  static int ellapsed_frames = 0;
  static char buffer[20] = "?? fps";
  static int numchars = 7;
  ellapsed_frames += 1;
  float seconds = (float)glfwGetTime(), ellapsed_seconds = seconds - old_seconds;
  if (ellapsed_seconds > 1.0f) {
    numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);
    old_seconds = seconds;
    ellapsed_frames = 0;
  }
  float lineheight = TextRendering_LineHeight(window), charwidth = TextRendering_CharWidth(window);
  TextRendering_PrintString(window, buffer, 1.0f - (numchars + 1) * charwidth, 1.0f - lineheight, 1.0f);
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
