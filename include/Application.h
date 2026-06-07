#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "objects/Asteroid.h"
#include "objects/Player.h"
#include "rendering/Shader.h"
#include "rendering/Texture.h"
#include "objects/TieDefender.h"
#include "objects/TieFighter.h"
#include "objects/TiePhantom.h"
#include "rendering/VertexArray.h"
#include "scene.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <type_traits>
#include <vector>

class Application {
public:
  Application();
  ~Application();

  bool Init();
  void LoadAssets(int argc, char *argv[]);
  void Run();
  void Shutdown();

  void DrawObject(
      const char *name,
      int id,
      const glm::mat4 &model,
      bool flip_normals = false
  );

  void DrawLine(glm::vec4 from, glm::vec4 to, int color_id);

  // Callbacks
  void KeyCallback(int key, int scancode, int action, int mod);
  void MouseButtonCallback(int button, int action, int mods);
  void CursorPosCallback(double xpos, double ypos);
  void ScrollCallback(double xoffset, double yoffset);
  void FramebufferSizeCallback(int width, int height);

  void SetCallbacks();
  void SetKeyCallback();
  void SetMouseButtonCallback();
  void SetCursorPosCallback();
  void SetMouseScrollCallback();
  void SetFramebufferSizeCallback();

  void SetProjectileColor(const glm::vec3 &color);

  void AddProjectile(std::unique_ptr<Projectile> projectile);
  std::vector<std::unique_ptr<Projectile>> &GetProjectiles() {
    return m_Projectiles;
  }

private:
  Shader *GetMainShader() const { return m_MainShader.get(); }
  GLFWwindow *m_Window;

  // Scene State
  std::map<std::string, SceneObject> m_VirtualScene;
  std::stack<glm::mat4> m_MatrixStack;
  float m_ScreenRatio = 1.0f;
  float m_AngleX = 0.0f;
  float m_AngleY = 0.0f;
  float m_AngleZ = 0.0f;

  // Input State
  bool m_LeftMouseButtonPressed = false;
  bool m_RightMouseButtonPressed = false;
  bool m_MiddleMouseButtonPressed = false;
  bool m_InvertY = true;
  double m_LastCursorPosX = 0.0, m_LastCursorPosY = 0.0;

  // Camera State
  enum class CameraMode { ThirdPerson, FirstPerson };
  CameraMode m_CameraMode = CameraMode::ThirdPerson;

  float m_CameraTheta = 0.0f;
  float m_CameraPhi = 0.0f;
  float m_CameraDistance = 3.5f;
  float m_CameraHeight = 1.0f;
  bool m_UsePerspectiveProjection = true;

  glm::vec4 m_CameraPosition = glm::vec4(0.0f, 0.0f, 3.5f, 1.0f);
  glm::vec4 m_CameraLookAt = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::vec4 m_CameraUp = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

  // Game Objects
  std::unique_ptr<Player> m_Player;
  std::vector<std::unique_ptr<Asteroid>> m_Asteroids;
  std::vector<std::unique_ptr<TieFighter>> m_TieFighters;
  std::vector<std::unique_ptr<TieDefender>> m_TieDefenders;
  std::vector<std::unique_ptr<TiePhantom>> m_TiePhantoms;
  std::vector<std::unique_ptr<Projectile>> m_Projectiles;

  // Game State
  bool m_GameOver = false;
  bool m_Victory = false;
  bool m_Paused = false;

  // Other State
  bool m_ShowInfoText = true;
  bool m_VsyncEnabled = true;
#if !RELEASE
  float m_DebugZoom = 1.0f;
#endif
  float m_ForearmAngleZ = 0.0f;
  float m_ForearmAngleX = 0.0f;
  float m_TorsoPositionX = 0.0f;
  float m_TorsoPositionY = 0.0f;

  // OpenGL Resources (RAII)
  std::unique_ptr<Shader> m_MainShader;
  GLuint m_LineVAO = 0;
  std::vector<std::unique_ptr<Texture>> m_Textures;
  std::vector<std::unique_ptr<VertexArray>> m_VertexArrays;
  std::vector<std::unique_ptr<VertexBuffer>> m_VertexBuffers;
  std::vector<std::unique_ptr<IndexBuffer>> m_IndexBuffers;

  void Update(float deltaTime);
  void Render();
  void RenderMinimap();
  void CheckCollisions();
  void Cleanup();

  std::vector<GameObject *> GetAllEnemies();
  std::vector<GameObject *> GetHarmfulObjects();

  // UI Helpers
  void TextRendering_ShowEulerAngles();
  void TextRendering_ShowProjection();
  void TextRendering_ShowFramesPerSecond();
  void TextRendering_ShowGameOver();
  void TextRendering_ShowVictory();

  void LoadModel(const char *path, const std::string &prefix = "");

  static GameObject *ToRaw(GameObject *p) { return p; }
  template <typename T> static GameObject *ToRaw(const std::unique_ptr<T> &p) {
    return p.get();
  }

  static bool
  SpheresIntersect(glm::vec4 posA, float rA, glm::vec4 posB, float rB);

  template <typename T, typename U, typename F>
  void CheckCollisions(T &a, U &b, F onCollisionCallback) {
    bool sameGroup = false;
    // Check if both containers are actually the same object to avoid double
    // counting
    if constexpr (std::is_same_v<T, U>) {
      if ((void *)&a == (void *)&b)
        sameGroup = true;
    }

    for (size_t i = 0; i < a.size(); ++i) {
      // If same group, start j from i+1 to avoid self-collision and duplicate
      // pairs
      size_t startJ = sameGroup ? i + 1 : 0;
      for (size_t j = startJ; j < b.size(); ++j) {
        auto *pA = ToRaw(a[i]);
        auto *pB = ToRaw(b[j]);
        if (pA != pB && SpheresIntersect(
                            pA->GetPosition(),
                            pA->GetRadius(),
                            pB->GetPosition(),
                            pB->GetRadius()
                        )) {
          onCollisionCallback(a[i], b[j]);
        }
      }
    }
  }

  template <typename T>
  void SpawnSquadrons(
      int numSquads,
      int unitsPerSquad,
      float distance,
      std::vector<std::unique_ptr<T>> &container
  ) {
    static float currentAngle = 0.0f;
    const float totalExpectedSquads =
        11.0f; // 6 Fighters + 3 Phantoms + 2 Defenders
    const float angleStep = (2.0f * 3.14159265f) / totalExpectedSquads;

    for (int s = 0; s < numSquads; ++s) {
      glm::vec4 center = glm::vec4(
          distance * cos(currentAngle),
          (rand() % 40) - 20.0f,
          distance * sin(currentAngle),
          1.0f
      );
      for (int i = 0; i < unitsPerSquad; ++i) {
        glm::vec4 offset = glm::vec4(
            (rand() % 10) - 5.0f,
            (rand() % 10) - 5.0f,
            (rand() % 10) - 5.0f,
            0.0f
        );
        container.push_back(std::make_unique<T>(center + offset));
      }
      currentAngle += angleStep;
    }
  }
};
#endif // APPLICATION_H
