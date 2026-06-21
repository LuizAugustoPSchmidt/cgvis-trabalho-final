#include "objects/TieDefender.h"
#include "Application.h"
#include "ObjectIds.h"
#include "glm/geometric.hpp"
#include "matrices.h"
#include "objects/Projectile.h"
#include "TieDefender.constants.h"
#include <cstdlib>

constexpr float ACCELERATION_MAX = 10.0f;
constexpr float SPEED_MAX = 30.0f;

TieDefender::TieDefender(glm::vec4 position)
    : GameObject("tiedefender_obj1", TIE_DEFENDER, "tie-defender"),
      m_Position(position) {
  m_ShootCooldown = static_cast<float>(rand() % 100) / 100.0f * TIE_DEFENDER_COOLDOWN;
}

void TieDefender::Update(float deltaTime) {
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

void TieDefender::Render(Application &app) {
  // Tie Defender: Size ~3000, Center ~ (0, 1120, 160)
  // Scale by 0.0006 to match TIE Fighter size.
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    m_RotationMatrix * Matrix_Scale(0.0006f, 0.0006f, 0.0006f) *
                    Matrix_Translate(0.0f, -1120.0f, -160.0f);
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, model);

#if !RELEASE
  // Debug Vectors
  app.DrawLine(m_Position, m_Position + m_Velocity, DEBUG_VECTOR_GREEN);
  app.DrawLine(m_Position, m_Position + m_Acceleration, DEBUG_VECTOR_RED);
#endif // !RELEASE
}

void TieDefender::Update(float deltaTime, Application &app) {
  Update(deltaTime);

  m_ShootCooldown -= deltaTime;
  if (m_ShootCooldown <= 0.0f) {
    float dist = glm::length(m_Position - m_Target);
    if (dist < TIE_DEFENDER_SHOOT_RANGE) {
      Shoot(app);
      m_ShootCooldown = TIE_DEFENDER_COOLDOWN;
    }
  }
}

void TieDefender::Shoot(Application &app) {
  glm::vec4 forward = GetForward();
  // Get right vector
  glm::vec4 right = glm::normalize(crossproduct(forward, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

  float speed = glm::length(m_Velocity);
  glm::vec4 velocity = forward * (speed + TIE_DEFENDER_SPEED_OFFSET);

  // Spawn left and right lasers
  glm::vec4 spawnPosLeft = m_Position - right * TIE_DEFENDER_LASER_OFFSET_X + forward * TIE_DEFENDER_LASER_OFFSET_Z;
  glm::vec4 spawnPosRight = m_Position + right * TIE_DEFENDER_LASER_OFFSET_X + forward * TIE_DEFENDER_LASER_OFFSET_Z;

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
