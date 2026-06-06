#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "scene.h"
#include <algorithm>

void Application::Render() {
  glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_MainShader->Use();

  // Set Lighting Uniforms
  m_MainShader->SetVec3("ambient_light_top", glm::vec3(0.1f, 0.1f, 0.15f));
  m_MainShader->SetVec3("ambient_light_bottom", glm::vec3(0.01f, 0.01f, 0.02f));

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
  for (auto &proj : m_Player->GetProjectiles())
    proj->Render(*this);
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
  TextRendering_ShowVictory();
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
