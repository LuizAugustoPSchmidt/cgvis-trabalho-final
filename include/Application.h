#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Asteroid.h"
#include "Player.h"
#include "Shader.h"
#include "Texture.h"
#include "TieDefender.h"
#include "TieFighter.h"
#include "TiePhantom.h"
#include "VertexArray.h"
#include "scene.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <memory>
#include <stack>
#include <string>
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

  // Callbacks
  void KeyCallback(int key, int scancode, int action, int mod);
  void MouseButtonCallback(int button, int action, int mods);
  void CursorPosCallback(double xpos, double ypos);
  void ScrollCallback(double xoffset, double yoffset);
  void FramebufferSizeCallback(int width, int height);

private:
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

  // Other State
  bool m_ShowInfoText = true;
  float m_ForearmAngleZ = 0.0f;
  float m_ForearmAngleX = 0.0f;
  float m_TorsoPositionX = 0.0f;
  float m_TorsoPositionY = 0.0f;

  // OpenGL Resources (RAII)
  std::unique_ptr<Shader> m_MainShader;
  std::vector<std::unique_ptr<Texture>> m_Textures;
  std::vector<std::unique_ptr<VertexArray>> m_VertexArrays;
  std::vector<std::unique_ptr<VertexBuffer>> m_VertexBuffers;
  std::vector<std::unique_ptr<IndexBuffer>> m_IndexBuffers;

  void Update(float deltaTime);
  void Render();

  // UI Helpers
  void TextRendering_ShowEulerAngles();
  void TextRendering_ShowProjection();
  void TextRendering_ShowFramesPerSecond();

  void LoadModel(const char *path, const std::string& prefix = "");
};
#endif // APPLICATION_H
