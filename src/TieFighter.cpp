#include "TieFighter.h"
#include "Application.h"
#include "ObjectIds.h"
#include "glm/geometric.hpp"
#include "matrices.h"

constexpr float ACCELERATION_MAX = 10.0f;
constexpr float SPEED_MAX = 30.0f;

TieFighter::TieFighter(glm::vec4 position)
    : GameObject("tiefighter_TIE_FIghter_Sphere.004", TIE_FIGHTER, "tie-fighter"),
      m_Position(position) {}

void TieFighter::Update(float deltaTime) {
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

void TieFighter::Render(Application &app) {
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    m_RotationMatrix * Matrix_Scale(0.5f, 0.5f, 0.5f);
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, model);

#if !RELEASE
  // Debug Vectors
  app.DrawLine(m_Position, m_Position + m_Velocity, DEBUG_VECTOR_GREEN);
  app.DrawLine(m_Position, m_Position + m_Acceleration, DEBUG_VECTOR_RED);
#endif // !RELEASE
}
