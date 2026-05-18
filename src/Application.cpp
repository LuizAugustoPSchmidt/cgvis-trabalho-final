#include "Application.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "scene.h"
#include "utils.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// External function for automated correction
void Correcao_KeyCallback(int key, int action, int mod);

Application::Application() : m_Window(nullptr) {}

Application::~Application() { Shutdown(); }

bool Application::Init() {
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: glfwInit() failed.\n");
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_Window =
      glfwCreateWindow(800, 600, "INF01047 - Caça nas Estrelas", NULL, NULL);
  if (!m_Window) {
    glfwTerminate();
    fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
    return false;
  }

  glfwSetWindowUserPointer(m_Window, this);

  glfwMakeContextCurrent(m_Window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *glversion = glGetString(GL_VERSION);
  const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  printf(
      "GPU: %s, %s, OpenGL %s, GLSL %s\n",
      vendor,
      renderer,
      glversion,
      glslversion
  );

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  return true;
}

#define SPACESHIP_MATERIAL 3
#define SPACESHIP_MOTOR 4
#define SPACESHIP_CASCO_ESCURO_1 5
#define SPACESHIP_CASCO 6
#define SPACESHIP_CSACO_ESCURO 7
#define SPACESHIP_FUNDO 8
#define SPACESHIP_TURBINA 9
#define SPACESHIP_FUNDO_2 10
#define SPACESHIP_QUEIMADO 11
#define SPACESHIP_PINTURA 12
#define SPACESHIP_LUZ_TURBINA 13
#define SPACESHIP_CABINE 14
#define SPACESHIP_PONTA 15
#define SPACESHIP_VIDRO 16
#define SPACESHIP_MATERIAL_001 17
#define BACKGROUND 18

void Application::LoadAssets(int argc, char *argv[]) {
  m_MainShader = std::make_unique<Shader>(
      "../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl"
  );

  m_MainShader->Use();
  m_MainShader->SetInt("TextureImage0", 0);
  m_MainShader->SetInt("TextureImage1", 1);
  m_MainShader->SetInt("TextureImage2", 2);

  m_Textures.push_back(
      std::make_unique<Texture>("../../data/red_brick_diff_1k.jpg", 0)
  );
  m_Textures.push_back(
      std::make_unique<Texture>("../../data/rocky_terrain_02_diff_1k.jpg", 1)
  );
  m_Textures.push_back(
      std::make_unique<Texture>("../../data/background.jpg", 2)
  );
  m_Textures.back()->SetWrapping(GL_MIRRORED_REPEAT);

  LoadModel("../../data/sphere.obj");
  LoadModel("../../data/spaceship.obj");

  m_SpaceshipParts = {
      {"Cube", SPACESHIP_MATERIAL},
      {"Cube_motor_0", SPACESHIP_MOTOR},
      {"asas_CASCO_ESCURO_1_0", SPACESHIP_CASCO_ESCURO_1},
      {"asas_Casco_0", SPACESHIP_CASCO},
      {"asas_csaco_escuro_0", SPACESHIP_CSACO_ESCURO},
      {"asas_fundo_0", SPACESHIP_FUNDO},
      {"asas_Turbina_0", SPACESHIP_TURBINA},
      {"asas_FUNDO_2_0", SPACESHIP_FUNDO_2},
      {"asas_queimado_0", SPACESHIP_QUEIMADO},
      {"asas_motor_0", SPACESHIP_MOTOR},
      {"asas_pintura_0", SPACESHIP_PINTURA},
      {"asas_luz_turbina_0", SPACESHIP_LUZ_TURBINA},
      {"casco_queimado_0", SPACESHIP_QUEIMADO},
      {"casco_Casco_0", SPACESHIP_CASCO},
      {"casco_Cabine_0", SPACESHIP_CABINE},
      {"casco_ponta_0", SPACESHIP_PONTA},
      {"casco_pintura_0", SPACESHIP_PINTURA},
      {"casco_vidro_0", SPACESHIP_VIDRO},
      {"casco_fundo_0", SPACESHIP_FUNDO},
      {"robo_motor_0", SPACESHIP_MOTOR},
      {"robo_and_0", SPACESHIP_MATERIAL},
      {"robo_Material.001_0", SPACESHIP_MATERIAL_001},
      {"robo_pintura_0", SPACESHIP_PINTURA},
  };

  if (argc > 1) {
    LoadModel(argv[1]);
  }

  TextRendering_Init();
}

void Application::LoadModel(const char *path) {
  ObjModel model(path);
  ComputeNormals(&model);
  BuildTrianglesAndAddToVirtualScene(&model, m_VirtualScene);
}

void Application::Run() {
  float lastTime = (float)glfwGetTime();
  while (!glfwWindowShouldClose(m_Window)) {
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    Update(deltaTime);
    Render();

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
  }
}

void Application::Update(float deltaTime) {
    if (m_CameraMode == CameraMode::ThirdPerson) {
        // TPV: Camera orbits the spaceship based on mouse drag
        float r = m_CameraDistance;
        float y = r * sin(m_CameraPhi);
        float z = r * cos(m_CameraPhi) * cos(m_CameraTheta);
        float x = r * cos(m_CameraPhi) * sin(m_CameraTheta);

        m_CameraPosition = m_SpaceshipPosition + glm::vec4(x, y, z, 0.0f);
        m_CameraLookAt = m_SpaceshipPosition;
        m_CameraUp = m_SpaceshipUp;
    } else {
        // FPV (Aim Mode): Camera is in the cockpit
        // We move the camera slightly forward (0.2) and up (0.2) from ship center
        m_CameraPosition = m_SpaceshipPosition + (m_SpaceshipForward * 0.2f) + (m_SpaceshipUp * 0.2f);
        m_CameraLookAt = m_CameraPosition + m_SpaceshipForward;
        m_CameraUp = m_SpaceshipUp;
    }
}

void Application::Render() {
  glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_MainShader->Use();

  glm::vec4 camera_view_vector = m_CameraLookAt - m_CameraPosition;
  glm::mat4 view = Matrix_Camera_View(m_CameraPosition, camera_view_vector, m_CameraUp);

  glm::mat4 projection;
  float nearplane = -0.01f; // Closer nearplane for FPV cockpit visibility
  float farplane = -100.0f; // Increased far plane for space

  if (m_UsePerspectiveProjection) {
    float field_of_view = 3.141592 / 3.0f;
    projection =
        Matrix_Perspective(field_of_view, m_ScreenRatio, nearplane, farplane);
  } else {
    float t = 1.5f * m_CameraDistance / 2.5f;
    float b = -t;
    float r = t * m_ScreenRatio;
    float l = -r;
    projection = Matrix_Orthographic(l, r, b, t, nearplane, farplane);
  }

  m_MainShader->SetMat4("view", view);
  m_MainShader->SetMat4("projection", projection);

  // Background skybox
  glDisable(GL_CULL_FACE);
  glm::mat4 model =
      Matrix_Translate(
          m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z
      ) *
      Matrix_Scale(50.0f, 50.0f, 50.0f);
  DrawObject("the_sphere", BACKGROUND, model);
  glEnable(GL_CULL_FACE);

  model = Matrix_Identity();

  // Spaceship
  // Since our spaceship faces some direction, we need to build its model matrix
  // correctly using position, forward and up vectors.
  // For now, let's just use its position and identity rotation if it's
  // stationary. We should eventually derive rotation from forward/up.
  model =
      Matrix_Translate(
          m_SpaceshipPosition.x, m_SpaceshipPosition.y, m_SpaceshipPosition.z
      ) *
      Matrix_Scale(0.1f, 0.1f, 0.1f);
  for (const auto &part : m_SpaceshipParts) {
    DrawObject(part.name, part.object_id, model);
  }

  TextRendering_ShowFramesPerSecond();
}

void Application::DrawObject(const char *name, int id, const glm::mat4 &model) {
  GLint bbox_min_uniform = m_MainShader->GetUniformLocation("bbox_min");
  GLint bbox_max_uniform = m_MainShader->GetUniformLocation("bbox_max");

  m_MainShader->SetMat4("model", model);
  m_MainShader->SetInt("object_id", id);
  DrawVirtualObject(name, m_VirtualScene, bbox_min_uniform, bbox_max_uniform);
}

void Application::TextRendering_ShowEulerAngles() {
  if (!m_ShowInfoText)
    return;
  float pad = TextRendering_LineHeight(m_Window);
  char buffer[80];
  snprintf(
      buffer,
      80,
      "Euler Angles rotation matrix = Z(%.2f)*Y(%.2f)*X(%.2f)\n",
      m_AngleZ,
      m_AngleY,
      m_AngleX
  );
  TextRendering_PrintString(
      m_Window, buffer, -1.0f + pad / 10, -1.0f + 2 * pad / 10, 1.0f
  );
}

void Application::TextRendering_ShowProjection() {
  if (!m_ShowInfoText)
    return;
  float lineheight = TextRendering_LineHeight(m_Window),
        charwidth = TextRendering_CharWidth(m_Window);
  if (m_UsePerspectiveProjection)
    TextRendering_PrintString(
        m_Window,
        "Perspective",
        1.0f - 13 * charwidth,
        -1.0f + 2 * lineheight / 10,
        1.0f
    );
  else
    TextRendering_PrintString(
        m_Window,
        "Orthographic",
        1.0f - 13 * charwidth,
        -1.0f + 2 * lineheight / 10,
        1.0f
    );
}

void Application::TextRendering_ShowFramesPerSecond() {
  if (!m_ShowInfoText)
    return;
  static float old_seconds = (float)glfwGetTime();
  static int ellapsed_frames = 0;
  static char buffer[20] = "?? fps";
  static int numchars = 7;
  ellapsed_frames += 1;
  float seconds = (float)glfwGetTime(),
        ellapsed_seconds = seconds - old_seconds;
  if (ellapsed_seconds > 1.0f) {
    numchars =
        snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);
    old_seconds = seconds;
    ellapsed_frames = 0;
  }
  float lineheight = TextRendering_LineHeight(m_Window),
        charwidth = TextRendering_CharWidth(m_Window);
  TextRendering_PrintString(
      m_Window,
      buffer,
      1.0f - (numchars + 1) * charwidth,
      1.0f - lineheight,
      1.0f
  );
}

void Application::Shutdown() {
  if (m_Window) {
    glfwTerminate();
    m_Window = nullptr;
  }
}

// Callback Implementations
void Application::KeyCallback(int key, int scancode, int action, int mod) {
  Correcao_KeyCallback(key, action, mod);
  if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
    glfwSetWindowShouldClose(m_Window, GL_TRUE);

  float delta = 3.141592 / 16;
  if (key == GLFW_KEY_X && action == GLFW_PRESS)
    m_AngleX += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    m_AngleY += (mod & GLFW_MOD_SHIFT) ? -delta : delta;
  if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    m_AngleZ += (mod & GLFW_MOD_SHIFT) ? -delta : delta;

  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
    m_AngleX = 0.0f;
    m_AngleY = 0.0f;
    m_AngleZ = 0.0f;
    m_ForearmAngleX = 0.0f;
    m_ForearmAngleZ = 0.0f;
    m_TorsoPositionX = 0.0f;
    m_TorsoPositionY = 0.0f;
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS)
    m_UsePerspectiveProjection = true;
  if (key == GLFW_KEY_O && action == GLFW_PRESS)
    m_UsePerspectiveProjection = false;
  if (key == GLFW_KEY_H && action == GLFW_PRESS)
    m_ShowInfoText = !m_ShowInfoText;
  if (key == GLFW_KEY_C && action == GLFW_PRESS)
    m_CameraMode = (m_CameraMode == CameraMode::ThirdPerson) ? CameraMode::FirstPerson : CameraMode::ThirdPerson;
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    // Shader reload logic could be moved here or kept in opengl_utils
    // For now, let's just re-instantiate our Shader object
    m_MainShader = std::make_unique<Shader>(
        "../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl"
    );
    fprintf(stdout, "Shaders recarregados!\n");
    fflush(stdout);
  }
}

void Application::MouseButtonCallback(int button, int action, int mods) {
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
    m_CameraPhi   += 0.01f * dy;

    float phimax = 3.141592f / 2;
    float phimin = -phimax;

    if (m_CameraPhi > phimax) m_CameraPhi = phimax;
    if (m_CameraPhi < phimin) m_CameraPhi = phimin;

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
  // Logic removed as requested
}

void Application::FramebufferSizeCallback(int width, int height) {
  glViewport(0, 0, width, height);
  m_ScreenRatio = (float)width / height;
}
