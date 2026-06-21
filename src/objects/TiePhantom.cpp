#include "objects/TiePhantom.h"
#include "Application.h"
#include "ObjectIds.h"
#include "glm/geometric.hpp"
#include "matrices.h"
#include "objects/Projectile.h"
#include "TiePhantom.constants.h"
#include <cstdlib>

constexpr float ACCELERATION_MAX = 10.0f;
constexpr float SPEED_MAX = 30.0f;

TiePhantom::TiePhantom(glm::vec4 position)
    : GameObject("tiephantom_mat0", TIE_PHANTOM_HULL, "tie-phantom"),
      m_Position(position) {
  m_Parts = {
      {"tiephantom_mat0", TIE_PHANTOM_HULL},
      {"tiephantom_mat1", TIE_PHANTOM_WINGS},
      {"tiephantom_mat2", TIE_PHANTOM_WINGS},
      {"tiephantom_mat3", TIE_PHANTOM_WINGS},
      {"tiephantom_mat4", TIE_PHANTOM_WINGS},
      {"tiephantom_mat5", TIE_PHANTOM_WINGS},
  };
  m_ShootCooldown = static_cast<float>(rand() % 100) / 100.0f * TIE_PHANTOM_COOLDOWN;
}

void TiePhantom::Update(float deltaTime) {
  // 1. Calculate Acceleration (Steering Force) towards the target
  glm::vec4 targetDir = m_Target - m_Position;
  float dist = glm::length(targetDir);
  if (dist > 0.001f) {
    m_Acceleration = (targetDir / dist) * ACCELERATION_MAX;
  }

  // 2. Physics Integration
  m_Velocity += m_Acceleration * deltaTime;

  float speed = glm::length(m_Velocity);
  if (speed > SPEED_MAX) {
    m_Velocity = (m_Velocity / speed) * SPEED_MAX;
  }
  m_Position += m_Velocity * deltaTime;

  // 3. Align Rotation Matrix with Velocity (Moving Forward)
  if (speed > 0.001f) {
    glm::vec4 forward = m_Velocity / speed;
    m_RotationMatrix = Matrix_Look_At(
        m_Position,
        m_Position + forward,
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
    );
  }
}

void TiePhantom::Render(Application &app) {
  // Tie Phantom: Size ~1.0, already mostly centered.
  // Scale by 2.0 to match TIE Fighter size.
  // Rotate 90 degrees around X to point forward.
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    m_RotationMatrix * Matrix_Scale(2.0f, 2.0f, 2.0f) *
                    Matrix_Rotate_X(3 * 3.141592f / 2.0f);
  for (const auto &part : m_Parts) {
    app.DrawObject(part.name.c_str(), part.object_id, model);
  }

#if !RELEASE
  // Debug Vectors
  app.DrawLine(m_Position, m_Position + m_Velocity, DEBUG_VECTOR_GREEN);
  app.DrawLine(m_Position, m_Position + m_Acceleration, DEBUG_VECTOR_RED);
#endif // !RELEASE
}

void TiePhantom::Update(float deltaTime, Application &app) {
  Update(deltaTime);

  m_ShootCooldown -= deltaTime;
  if (m_ShootCooldown <= 0.0f) {
    float dist = glm::length(m_Position - m_Target);
    if (dist < TIE_PHANTOM_SHOOT_RANGE) {
      Shoot(app);
      m_ShootCooldown = TIE_PHANTOM_COOLDOWN;
    }
  }
}

void TiePhantom::Shoot(Application &app) {
  glm::vec4 forward = GetForward();
  // Get right vector
  glm::vec4 right = glm::normalize(crossproduct(forward, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

  float speed = glm::length(m_Velocity);
  glm::vec4 velocity = forward * (speed + TIE_PHANTOM_SPEED_OFFSET);

  // Spawn left and right lasers
  glm::vec4 spawnPosLeft = m_Position - right * TIE_PHANTOM_LASER_OFFSET_X + forward * TIE_PHANTOM_LASER_OFFSET_Z;
  glm::vec4 spawnPosRight = m_Position + right * TIE_PHANTOM_LASER_OFFSET_X + forward * TIE_PHANTOM_LASER_OFFSET_Z;

  app.AddProjectile(std::make_unique<Projectile>(
      spawnPosLeft,
      velocity,
      glm::vec3(0.0f, 1.0f, 0.0f), // green
      true // isEnemy
  ));
  app.AddProjectile(std::make_unique<Projectile>(
      spawnPosRight,
      velocity,
      glm::vec3(0.0f, 1.0f, 0.0f), // green
      true // isEnemy
  ));
}
