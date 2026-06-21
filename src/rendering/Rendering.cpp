#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"
#include "opengl_utils.h"
#include "Application.constants.h"
#include <algorithm>

void Application::Render() {
  glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_MainShader->Use();

  // Set Lighting Uniforms
  m_MainShader->SetVec3("ambient_light_top", glm::vec3(0.1f, 0.1f, 0.15f));
  m_MainShader->SetVec3("ambient_light_bottom", glm::vec3(0.01f, 0.01f, 0.02f));

  m_MainShader->SetVec4("star_direction", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
  m_MainShader->SetVec3("star_diffuse_color", glm::vec3(1.0f, 1.0f, 0.8f));
  m_MainShader->SetVec3("star_specular_color", glm::vec3(1.0f, 1.0f, 1.0f));

  m_MainShader->SetVec3("Ks", glm::vec3(0.5f, 0.5f, 0.5f));
  m_MainShader->SetFloat("shininess", 32.0f);

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

  // Set Projectile Lights
  // In the future, collect from TIEs as well
  int numLights = std::min((int)m_Projectiles.size(), 40);
  m_MainShader->SetInt("num_projectile_lights", numLights);
  for (int i = 0; i < numLights; ++i) {
    std::string base = "projectile_lights[" + std::to_string(i) + "].";
    m_MainShader->SetVec4(base + "start", m_Projectiles[i]->GetStartPoint());
    m_MainShader->SetVec4(base + "end", m_Projectiles[i]->GetEndPoint());
    m_MainShader->SetVec3(base + "color", m_Projectiles[i]->GetColor());
  }

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

  if (m_StartScreen) {
    TextRendering_ShowStartScreen();
    return;
  }

  // Render Game Objects
  m_Player->Render(*this);
  for (auto &proj : m_Projectiles)
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
  TextRendering_ShowPauseScreen();
}

void Application::RenderMinimap() {
  // 1. Get current window dimensions
  int width, height;
  glfwGetFramebufferSize(m_Window, &width, &height);

  glDisable(GL_CULL_FACE);

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

  // Player blip
  DrawTriangleBlip(playerPos, m_Player->GetForward(), DEBUG_VECTOR_GREEN, MINIMAP_PLAYER_BLIP_SIZE);

  // Enemy blips
  for (const auto &ship : m_TieFighters)
    DrawTriangleBlip(
        ship->GetPosition(),
        ship->GetForward(),
        DEBUG_VECTOR_RED,
        MINIMAP_ENEMY_BLIP_SIZE
    );
  for (const auto &ship : m_TieDefenders)
    DrawTriangleBlip(
        ship->GetPosition(),
        ship->GetForward(),
        DEBUG_VECTOR_RED,
        MINIMAP_ENEMY_BLIP_SIZE
    );
  for (const auto &ship : m_TiePhantoms)
    DrawTriangleBlip(
        ship->GetPosition(),
        ship->GetForward(),
        DEBUG_VECTOR_RED,
        MINIMAP_ENEMY_BLIP_SIZE
    );

  // 5. Restore Main Viewport
  glViewport(0, 0, width, height);
  glEnable(GL_CULL_FACE);
}

void Application::DrawTriangleBlip(glm::vec4 pos, glm::vec4 forward, int color, float size) {
  glm::vec4 F = forward;
  F.y = 0.0f; // Project to XZ plane
  float len = norm(F);
  if (len > 0.001f) {
    F /= len;
  } else {
    F = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
  }

  // Render the custom triangle model on the GPU
  glm::mat4 model = Matrix_Translate(pos.x, pos.y, pos.z) *
                    Matrix_Look_At(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), F, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) *
                    Matrix_Scale(size, size, size);

  DrawObject("minimap_triangle", color, model);
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

void Application::SetProjectileColor(const glm::vec3 &color) {
  m_MainShader->SetVec3("projectile_color", color);
}
