#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "scene.h"
// #include "utils.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
// #include <iostream>

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

  glfwSwapInterval(m_VsyncEnabled ? 1 : 0);

  return true;
}

void Application::LoadAssets(int argc, char *argv[]) {
#if RELEASE
  const char *backgrounds[] = {
      "../../data/background/background-texture.jpg",
      "../../data/background/background-texture-2k.jpg"};

  const int random = rand();
  const int random_bg_index = random % 2;
  printf("random bg index %d\n", random);

  const char *bg = backgrounds[random_bg_index];
#else
  const char *bg = "../../data/background/bg-white.jpg";
#endif
  m_MainShader = std::make_unique<Shader>(
      "../../src/shader_vertex.glsl", "../../src/shader_fragment.glsl"
  );

  m_MainShader->Use();
  m_MainShader->SetInt("TextureImage0", 0);
  m_MainShader->SetInt("TextureImage1", 1);
  m_MainShader->SetInt("TextureImage2", 2);
  m_MainShader->SetInt("TextureImage3", 3);
  m_MainShader->SetInt("TextureImage4", 4);
  m_MainShader->SetInt("TextureImage5", 5);
  m_MainShader->SetInt("TextureImage6", 6);
  m_MainShader->SetInt("TextureImage7", 7);

  m_Textures.push_back(
      std::make_unique<Texture>("../../data/opponent/opponent-texture.jpg", 0)
  );

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/asteroid/asteroid-texture-alt.jpg", 1
      )
  );

  m_Textures.push_back(std::make_unique<Texture>(bg, 2));

  m_Textures.back()->SetWrapping(GL_MIRRORED_REPEAT);

  m_Textures.push_back(
      std::make_unique<Texture>("../../data/asteroid/asteroid-texture.jpg", 3)
  );

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-fighter/tie-fighter-texture.png", 4
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-defender/tie-defender-texture.png", 5
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);
  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-phantom/tie-phantom-texture.jpeg", 6
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-phantom/tie-phantom-texture-wings.jpeg", 7
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);

  LoadModel("../../data/background/background-model.obj");
  LoadModel("../../data/player/player-model.obj");
  LoadModel("../../data/asteroid/asteroid-model.obj");
  LoadModel("../../data/tie-fighter/tie-fighter-model.obj", "tiefighter_");
  LoadModel("../../data/tie-defender/tie-defender-model.obj", "tiedefender_");
  LoadModel("../../data/tie-phantom/tie-phantom-model.obj", "tiephantom_");

  m_Player = std::make_unique<Player>();

  m_Asteroids.push_back(
      std::make_unique<Asteroid>(glm::vec4(15.0f, 2.0f, -20.0f, 1.0f))
  );
  m_Asteroids.push_back(
      std::make_unique<Asteroid>(
          glm::vec4(-25.0f, 10.0f, -30.0f, 1.0f),
          glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
          Matrix_Rotate_Y(1.0f)
      )
  );
  m_Asteroids.push_back(
      std::make_unique<Asteroid>(
          glm::vec4(5.0f, -15.0f, -45.0f, 1.0f),
          glm::vec4(2.5f, 2.5f, 2.5f, 0.0f)
      )
  );
  m_Asteroids.push_back(
      std::make_unique<Asteroid>(
          glm::vec4(-35.0f, -5.0f, 10.0f, 1.0f),
          glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
          Matrix_Rotate_X(0.5f)
      )
  );

  // For debug: render ships around the player's x-wing
  float radius = 20.0f;
  int totalShips = 7;
  for (int i = 0; i < totalShips; ++i) {
    float angle = i * (2.0f * 3.14159265f / totalShips);
    glm::vec4 pos =
        glm::vec4(radius * sin(angle), 0.0f, radius * cos(angle), 1.0f);

    if (i < 3) {
      m_TieFighters.push_back(std::make_unique<TieFighter>(pos));
    } else if (i < 5) {
      m_TieDefenders.push_back(std::make_unique<TieDefender>(pos));
    } else {
      m_TiePhantoms.push_back(std::make_unique<TiePhantom>(pos));
    }
  }

  if (argc > 1) {
    LoadModel(argv[1]);
  }

  TextRendering_Init();
}

void Application::LoadModel(const char *path, const std::string &prefix) {
  ObjModel model(path);
  ComputeNormals(&model);
  BuildTrianglesAndAddToVirtualScene(&model, m_VirtualScene, prefix);
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
  m_Player->SetRotation(m_CameraTheta);
  m_Player->Update(deltaTime);
  for (auto &asteroid : m_Asteroids)
    asteroid->Update(deltaTime);
  for (auto &ship : m_TieFighters) {
    ship->SetTarget(m_Player->GetPosition());
    ship->Update(deltaTime);
  }
  for (auto &ship : m_TieDefenders) {
    ship->SetTarget(m_Player->GetPosition());
    ship->Update(deltaTime);
  }
  for (auto &ship : m_TiePhantoms) {
    ship->SetTarget(m_Player->GetPosition());
    ship->Update(deltaTime);
  }

  if (m_CameraMode == CameraMode::ThirdPerson) {
    // TPV: Camera orbits the spaceship based on mouse drag
    float r = m_CameraDistance;
    float y = r * sin(m_CameraPhi);
    float z = r * cos(m_CameraPhi) * cos(m_CameraTheta);
    float x = r * cos(m_CameraPhi) * sin(m_CameraTheta);

    m_CameraPosition = m_Player->GetPosition() + glm::vec4(x, y, z, 0.0f);
    m_CameraLookAt = m_Player->GetPosition();
    m_CameraUp = m_Player->GetUp();
  } else {
    // FPV (Aim Mode): Camera is in the cockpit
    // We move the camera slightly forward (0.2) and up (0.2) from ship center
    m_CameraPosition = m_Player->GetPosition() +
                       (m_Player->GetForward() * 0.2f) +
                       (m_Player->GetUp() * 0.2f);
    m_CameraLookAt = m_CameraPosition + m_Player->GetForward();
    m_CameraUp = m_Player->GetUp();
  }
}

void Application::Render() {
  glClearColor(0.9f, 0.9f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_MainShader->Use();

  glm::vec4 camera_view_vector = m_CameraLookAt - m_CameraPosition;
  glm::mat4 view =
      Matrix_Camera_View(m_CameraPosition, camera_view_vector, m_CameraUp);

  glm::mat4 projection;
  float nearplane = -0.01f; // Closer nearplane for FPV cockpit visibility
  float farplane = -100.0f; // Increased far plane for space

  if (m_UsePerspectiveProjection) {
    float field_of_view = 3.141592 / 3.0f;
#if !RELEASE
    field_of_view /= m_DebugZoom;
#endif
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
  DrawObject("the_sphere", BACKGROUND, model, false);
  glEnable(GL_CULL_FACE);

  // Render Game Objects
  m_Player->Render(*this);
  for (auto &asteroid : m_Asteroids)
    asteroid->Render(*this);
  for (auto &ship : m_TieFighters)
    ship->Render(*this);
  for (auto &ship : m_TieDefenders)
    ship->Render(*this);
  for (auto &ship : m_TiePhantoms)
    ship->Render(*this);

  TextRendering_ShowFramesPerSecond();
}

void Application::DrawObject(
    const char *name, int id, const glm::mat4 &model, bool flip_normals
) {
  GLint bbox_min_uniform = m_MainShader->GetUniformLocation("bbox_min");
  GLint bbox_max_uniform = m_MainShader->GetUniformLocation("bbox_max");

  m_MainShader->SetMat4("model", model);
  m_MainShader->SetInt("object_id", id);
  m_MainShader->SetBool("flip_normals", flip_normals);
  DrawVirtualObject(name, m_VirtualScene, bbox_min_uniform, bbox_max_uniform);
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

  float x = 1.0f - (numchars + 1) * charwidth;
  float y = 1.0f - lineheight;
  float rect_w = (numchars + 1) * charwidth;
  float rect_h = lineheight;

  TextRendering_DrawRectangle(
      m_Window,
      x,
      y + rect_h / 1.25f,
      rect_w,
      rect_h,
      glm::vec4(0.0f, 0.0f, 0.0f, 0.5f)
  );
  TextRendering_PrintString(
      m_Window, buffer, x, y, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
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

  if (key == GLFW_KEY_C && action == GLFW_PRESS)
    m_CameraMode = (m_CameraMode == CameraMode::ThirdPerson)
                       ? CameraMode::FirstPerson
                       : CameraMode::ThirdPerson;
  if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    m_VsyncEnabled = !m_VsyncEnabled;
    glfwSwapInterval(m_VsyncEnabled ? 1 : 0);
    printf("V-Sync: %s\n", m_VsyncEnabled ? "ON" : "OFF");
  }
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

    if (m_CameraMode == CameraMode::ThirdPerson) {
      m_CameraPhi += 0.01f * dy;

      float phimax = 3.141592f / 2;
      float phimin = -phimax;

      if (m_CameraPhi > phimax)
        m_CameraPhi = phimax;
      if (m_CameraPhi < phimin)
        m_CameraPhi = phimin;
    }

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
#if !RELEASE
  m_DebugZoom += (float)yoffset * 0.1f;
  if (m_DebugZoom < 0.6f)
    m_DebugZoom = 0.6f;
  printf("Debug Zoom: %.1f\n", m_DebugZoom);
#endif
}

void Application::FramebufferSizeCallback(int width, int height) {
  glViewport(0, 0, width, height);
  m_ScreenRatio = (float)width / height;
}
