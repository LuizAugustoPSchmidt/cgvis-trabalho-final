#include "Application.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "scene.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <glm/gtc/type_ptr.hpp>

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
  LoadModel("../../data/others/projectile-model.obj", "laser_");

  m_Player = std::make_unique<Player>();

  for (int i = 0; i < 80; ++i) {
    float r = 30.0f + static_cast<float>(rand() % 170); // 30 to 200 units away
    float theta = static_cast<float>(rand() % 100) / 100.0f * 2.0f * 3.14159f;
    float phi = static_cast<float>(rand() % 100) / 100.0f * 3.14159f;

    glm::vec4 pos(
        r * sin(phi) * cos(theta),
        r * sin(phi) * sin(theta),
        r * cos(phi),
        1.0f
    );

    // Random scale between 0.5 and 3.0
    float s = 0.5f + static_cast<float>(rand() % 250) / 100.0f;
    glm::vec4 scale(s, s, s, 0.0f);

    // Random rotation
    float rotAngle =
        static_cast<float>(rand() % 100) / 100.0f * 2.0f * 3.14159f;
    glm::mat4 rotation = Matrix_Rotate(
        rotAngle,
        glm::normalize(
            glm::vec4(
                static_cast<float>(rand() % 10 - 5),
                static_cast<float>(rand() % 10 - 5),
                static_cast<float>(rand() % 10 - 5),
                0.0f
            )
        )
    );

    m_Asteroids.push_back(std::make_unique<Asteroid>(pos, scale, rotation));
  }

  // Spawn TIE Squadrons
  SpawnSquadrons(6, 12, 100.0f, m_TieFighters); // 6 squads of 12 Fighters
  SpawnSquadrons(3, 6, 150.0f, m_TiePhantoms);  // 3 squads of 6 Phantoms
  SpawnSquadrons(2, 3, 200.0f, m_TieDefenders); // 2 squads of 3 Defenders

  if (argc > 1) {
    LoadModel(argv[1]);
  }

  CreateMinimapTriangle();

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

void Application::AddProjectile(std::unique_ptr<Projectile> projectile) {
  m_Projectiles.push_back(std::move(projectile));
}

void Application::Update(float deltaTime) {
  if (m_StartScreen)
    return;

  m_Player->SetTheta(m_CameraTheta);
  m_Player->SetPhi(m_CameraPhi);
  m_Player->UpdateOrientation();

  if (!m_Paused && !m_Victory) {
    m_Player->Update(deltaTime);
    for (auto &asteroid : m_Asteroids)
      asteroid->Update(deltaTime);
    for (auto &ship : m_TieFighters) {
      ship->SetTarget(m_Player->GetPosition());
      ship->Update(deltaTime, *this);
    }
    for (auto &ship : m_TieDefenders) {
      ship->SetTarget(m_Player->GetPosition());
      ship->Update(deltaTime, *this);
    }
    for (auto &ship : m_TiePhantoms) {
      ship->SetTarget(m_Player->GetPosition());
      ship->Update(deltaTime, *this);
    }

    // Update Projectiles
    for (auto &p : m_Projectiles)
      p->Update(deltaTime);

    if (!m_GameOver)
      CheckCollisions();

    Cleanup();
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

void Application::Cleanup() {
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
  cleanup(m_Projectiles);

  if (m_TieFighters.empty() && m_TieDefenders.empty() && m_TiePhantoms.empty())
    m_Victory = true;
}

void Application::Reset() {
  m_GameOver = false;
  m_Victory = false;
  m_Paused = false;
  m_StartScreen = false;
  m_SpawnAngle = 0.0f;

  m_Player = std::make_unique<Player>();

  m_TieFighters.clear();
  m_TieDefenders.clear();
  m_TiePhantoms.clear();

  SpawnSquadrons(6, 12, 100.0f, m_TieFighters);
  SpawnSquadrons(3, 6, 150.0f, m_TiePhantoms);
  SpawnSquadrons(2, 3, 200.0f, m_TieDefenders);
}

void Application::Shutdown() {
  if (m_Window) {
    glfwTerminate();
    m_Window = nullptr;
  }
}

std::vector<GameObject *> Application::GetAllEnemies() {
  std::vector<GameObject *> enemies;
  for (auto &s : m_TieFighters)
    enemies.push_back(s.get());
  for (auto &s : m_TieDefenders)
    enemies.push_back(s.get());
  for (auto &s : m_TiePhantoms)
    enemies.push_back(s.get());
  return enemies;
}

std::vector<GameObject *> Application::GetHarmfulObjects() {
  std::vector<GameObject *> harmful = GetAllEnemies();
  for (auto &a : m_Asteroids)
    harmful.push_back(a.get());
  return harmful;
}

void Application::CreateMinimapTriangle() {
  // Flat unit equilateral triangle in XZ plane pointing along +Z
  std::vector<float> positions = {
      0.0f,
      0.0f,
      1.0f,
      1.0f, // 0: Tip
      -0.866025f,
      0.0f,
      -0.5f,
      1.0f, // 1: Left
      0.866025f,
      0.0f,
      -0.5f,
      1.0f // 2: Right
  };

  std::vector<float> normals =
      {0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

  std::vector<float> texcoords = {0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};

  std::vector<unsigned int> indices = {0, 2, 1};

  GLuint vao_id;
  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);

  auto upload_vbo =
      [](GLuint location, GLint size, const std::vector<float> &data) {
        GLuint vbo_id;
        glGenBuffers(1, &vbo_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
        glBufferData(
            GL_ARRAY_BUFFER,
            data.size() * sizeof(float),
            data.data(),
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
      };

  upload_vbo(0, 4, positions);
  upload_vbo(1, 4, normals);
  upload_vbo(2, 2, texcoords);

  GLuint ebo_id;
  glGenBuffers(1, &ebo_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      indices.size() * sizeof(unsigned int),
      indices.data(),
      GL_STATIC_DRAW
  );

  glBindVertexArray(0);

  SceneObject theobject;
  theobject.name = "minimap_triangle";
  theobject.first_index = 0;
  theobject.num_indices = 3;
  theobject.rendering_mode = GL_TRIANGLES;
  theobject.vertex_array_object_id = vao_id;
  theobject.bbox_min = glm::vec3(-0.866025f, 0.0f, -0.5f);
  theobject.bbox_max = glm::vec3(0.866025f, 0.0f, 1.0f);

  m_VirtualScene["minimap_triangle"] = theobject;
}