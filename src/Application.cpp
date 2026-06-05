#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "scene.h"
// #include "utils.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
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
      "../../src/shader_vertex.glsl",
      "../../src/shader_fragment.glsl"
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
          "../../data/asteroid/asteroid-texture-alt.jpg",
          1
      )
  );

  m_Textures.push_back(std::make_unique<Texture>(bg, 2));

  m_Textures.back()->SetWrapping(GL_MIRRORED_REPEAT);

  m_Textures.push_back(
      std::make_unique<Texture>("../../data/asteroid/asteroid-texture.jpg", 3)
  );

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-fighter/tie-fighter-texture.png",
          4
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-defender/tie-defender-texture.png",
          5
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);
  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-phantom/tie-phantom-texture.jpeg",
          6
      )
  );
  m_Textures.back()->SetWrapping(GL_REPEAT);

  m_Textures.push_back(
      std::make_unique<Texture>(
          "../../data/tie-phantom/tie-phantom-texture-wings.jpeg",
          7
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

  // Spawn TIE Squadrons
  SpawnSquadrons(6, 12, 100.0f, m_TieFighters); // 6 squads of 12 Fighters
  SpawnSquadrons(3, 6, 150.0f, m_TiePhantoms);  // 3 squads of 6 Phantoms
  SpawnSquadrons(2, 3, 200.0f, m_TieDefenders); // 2 squads of 3 Defenders

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
  m_Player->SetTheta(m_CameraTheta);
  m_Player->SetPhi(m_CameraPhi);
  m_Player->UpdateOrientation();

  if (!m_Paused) {
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

    if (!m_GameOver)
      CheckCollisions();
  }

  if (m_CameraMode == CameraMode::ThirdPerson) {
    // TPV: Camera follows behind the ship
    float r = m_CameraDistance;
    m_CameraPosition = m_Player->GetPosition() - m_Player->GetForward() * r +
                       m_Player->GetUp() * (r * 0.3f);
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
  float nearplane = -0.01f;  // Closer nearplane for FPV cockpit visibility
  float farplane = -2000.0f; // Increased far plane for space

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
  glDepthMask(GL_FALSE);
  glm::mat4 model = Matrix_Translate(
                        m_CameraPosition.x,
                        m_CameraPosition.y,
                        m_CameraPosition.z
                    ) *
                    Matrix_Scale(1000.0f, 1000.0f, 1000.0f);
  DrawObject("the_sphere", BACKGROUND, model, false);
  glDepthMask(GL_TRUE);
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

  RenderMinimap();

  TextRendering_ShowFramesPerSecond();
  TextRendering_ShowGameOver();
}

void Application::RenderMinimap() {
  // 1. Get current window dimensions
  int width, height;
  glfwGetFramebufferSize(m_Window, &width, &height);

  // 2. Define Minimap Viewport (Bottom Right)
  int minimapSize = std::min(width, height) / 4;
  int margin = 20;
  glViewport(width - minimapSize - margin, margin, minimapSize, minimapSize);

  // 3. Set up Top-Down Camera
  glm::vec4 playerPos = m_Player->GetPosition();
  float zoom = 300.0f; // Radius of world visible in minimap
  glm::vec4 cameraPos = playerPos + glm::vec4(0.0f, 500.0f, 0.0f, 0.0f);
  glm::vec4 lookAt = playerPos;
  glm::vec4 viewUp = glm::vec4(
      0.0f,
      0.0f,
      -1.0f,
      0.0f
  ); // Map "Forward" (-Z) to "Up" on screen

  glm::mat4 view = Matrix_Camera_View(cameraPos, lookAt - cameraPos, viewUp);
  glm::mat4 projection =
      Matrix_Orthographic(-zoom, zoom, -zoom, zoom, -0.1f, -1000.0f);

  m_MainShader->SetMat4("view", view);
  m_MainShader->SetMat4("projection", projection);

  // 4. Render Blips
  glClear(GL_DEPTH_BUFFER_BIT);

  auto drawArrowBlip = [&](glm::vec4 pos,
                           glm::vec4 forward,
                           int color,
                           float size) {
    // Base dot
    DrawObject(
        "the_sphere",
        color,
        Matrix_Translate(pos.x, pos.y, pos.z) * Matrix_Scale(size, size, size)
    );
    // Orientation line
    DrawLine(pos, pos + forward * size * 3.0f, color);
  };

  // Player arrow
  drawArrowBlip(playerPos, m_Player->GetForward(), DEBUG_VECTOR_GREEN, 10.0f);

  // Enemy arrows
  for (const auto &ship : m_TieFighters)
    drawArrowBlip(
        ship->GetPosition(),
        ship->GetForward(),
        DEBUG_VECTOR_RED,
        5.0f
    );
  for (const auto &ship : m_TieDefenders)
    drawArrowBlip(
        ship->GetPosition(),
        ship->GetForward(),
        DEBUG_VECTOR_RED,
        5.0f
    );
  for (const auto &ship : m_TiePhantoms)
    drawArrowBlip(
        ship->GetPosition(),
        ship->GetForward(),
        DEBUG_VECTOR_RED,
        5.0f
    );

  // 5. Restore Main Viewport
  glViewport(0, 0, width, height);
}

void Application::DrawObject(
    const char *name,
    int id,
    const glm::mat4 &model,
    bool flip_normals
) {
  GLint bbox_min_uniform = m_MainShader->GetUniformLocation("bbox_min");
  GLint bbox_max_uniform = m_MainShader->GetUniformLocation("bbox_max");

  m_MainShader->SetMat4("model", model);
  m_MainShader->SetInt("object_id", id);
  m_MainShader->SetBool("flip_normals", flip_normals);
  DrawVirtualObject(name, m_VirtualScene, bbox_min_uniform, bbox_max_uniform);
}

void Application::DrawLine(glm::vec4 from, glm::vec4 to, int color_id) {
  glm::vec4 direction = to - from;
  float length = norm(direction);
  if (length < 0.01f)
    return;

  // Matrix_Look_At aligns +Z with (to - from)
  // We scale Z by length and X,Y by a small value for thickness.
  // We use the_sphere as our geometry for the line.
  glm::mat4 model =
      Matrix_Translate(from.x, from.y, from.z) *
      Matrix_Look_At(from, to, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) *
      Matrix_Scale(0.1f, 0.1f, length);

  DrawObject("the_sphere", color_id, model);
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
      m_Window,
      buffer,
      x,
      y,
      1.0f,
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
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
  if (key == GLFW_KEY_W) {
    if (action == GLFW_PRESS)
      m_Player->SetBoosting(true);
    else if (action == GLFW_RELEASE)
      m_Player->SetBoosting(false);
  }
  if (key == GLFW_KEY_I && action == GLFW_PRESS) {
    m_InvertY = !m_InvertY;
#if !RELEASE
    printf(
        "Mouse Y Inversion: %s\n",
        m_InvertY ? "ON (Flight)" : "OFF (Normal)"
    );
#endif
  }
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    m_Paused = !m_Paused;
    printf("Game %s\n", m_Paused ? "PAUSED" : "RESUMED");
  }
  if (key == GLFW_KEY_V && action == GLFW_PRESS) {
    m_VsyncEnabled = !m_VsyncEnabled;
    glfwSwapInterval(m_VsyncEnabled ? 1 : 0);
    printf("V-Sync: %s\n", m_VsyncEnabled ? "ON" : "OFF");
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    // Shader reload logic could be moved here or kept in opengl_utils
    // For now, let's just re-instantiate our Shader object
    m_MainShader = std::make_unique<Shader>(
        "../../src/shader_vertex.glsl",
        "../../src/shader_fragment.glsl"
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

    m_CameraPhi += 0.01f * (m_InvertY ? dy : -dy);

    float phimax = 3.141592f / 2;
    float phimin = -phimax;

    if (m_CameraPhi > phimax)
      m_CameraPhi = phimax;
    if (m_CameraPhi < phimin)
      m_CameraPhi = phimin;

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

bool Application::SphereSphere(
    glm::vec4 posA, float rA, glm::vec4 posB, float rB
) {
  glm::vec4 d = posA - posB;
  float dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
  float rSum = rA + rB;
  return dist2 < rSum * rSum;
}

void Application::CheckCollisions() {
  // 1. Player vs Harmful (Enemies + Asteroids)
  std::vector<GameObject *> harmfulObjects;
  for (auto &a : m_Asteroids) harmfulObjects.push_back(a.get());
  for (auto &s : m_TieFighters) harmfulObjects.push_back(s.get());
  for (auto &s : m_TieDefenders) harmfulObjects.push_back(s.get());
  for (auto &s : m_TiePhantoms) harmfulObjects.push_back(s.get());

  std::vector<GameObject *> playerVec = {m_Player.get()};

  CheckCollisions(playerVec, harmfulObjects, [&](auto &p, auto &h) {
    m_GameOver = true;
  });

  if (m_GameOver)
    return;

  // 2. Enemies vs Asteroids
  auto enemyVsAsteroids = [&](auto &enemies) {
    CheckCollisions(enemies, m_Asteroids, [](auto &s, auto &a) {
#if !RELEASE
      std::cout << s->GetClassId() << " hit an asteroid" << std::endl;
#endif // !RELEASE
      s->Kill();
    });
  };
  enemyVsAsteroids(m_TieFighters);
  enemyVsAsteroids(m_TieDefenders);
  enemyVsAsteroids(m_TiePhantoms);

  // 3. Enemy vs Enemy
  std::vector<GameObject *> allEnemies;
  for (auto &s : m_TieFighters) allEnemies.push_back(s.get());
  for (auto &s : m_TieDefenders) allEnemies.push_back(s.get());
  for (auto &s : m_TiePhantoms) allEnemies.push_back(s.get());

  CheckCollisions(allEnemies, allEnemies, [](auto &s1, auto &s2) {
#if !RELEASE
    std::cout << s1->GetClassId() << " and " << s2->GetClassId() << " hit. Both dead" << std::endl;
#endif // !RELEASE
    s1->Kill();
    s2->Kill();
  });

  // 4. Cleanup dead ships
  auto cleanup = [](auto &vec) {
    vec.erase(
        std::remove_if(
            vec.begin(),
            vec.end(),
            [](const auto &s) { return s->IsDead(); }
        ),
        vec.end()
    );
  };
  cleanup(m_TieFighters);
  cleanup(m_TieDefenders);
  cleanup(m_TiePhantoms);

  // 5. Asteroid vs Asteroid
  CheckCollisions(m_Asteroids, m_Asteroids, [](auto &a1, auto &a2) {
    a1->ReverseDirection();
    a2->ReverseDirection();
  });
}

void Application::TextRendering_ShowGameOver() {
  if (!m_GameOver)
    return;

  const char *msg = "GAME OVER";
  int numchars = 9;
  float lineheight = TextRendering_LineHeight(m_Window);
  float charwidth = TextRendering_CharWidth(m_Window);

  float x = -(numchars / 2.0f) * charwidth * 2.0f;
  float y = 0.0f;

  TextRendering_PrintString(
      m_Window,
      msg,
      x,
      y,
      2.0f,
      glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
  );
}
