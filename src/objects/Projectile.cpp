#include "objects/Projectile.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

Projectile::Projectile(glm::vec4 position, glm::vec4 velocity, glm::vec3 color, bool isEnemy)
    : GameObject("laser_Capsule", LASER_BOLT, "projectile"),
      m_Position(position), m_Velocity(velocity), m_Color(color), m_IsEnemy(isEnemy) {}

void Projectile::Update(float deltaTime) {
  m_Position += m_Velocity * deltaTime;
  m_TimeAlive += deltaTime;
  if (m_TimeAlive >= MAX_LIFETIME)
    Kill();
}

void Projectile::Render(Application &app) {
  float speed = glm::length(m_Velocity);
  if (speed < 0.001f)
    return;

  glm::vec4 forward = m_Velocity / speed;

  // The capsule is generated along the Y-axis.
  // We need to rotate it to point towards forward (+Z in local space of
  // Matrix_Look_At) Actually Matrix_Look_At already handles the orientation. We
  // just need to rotate the mesh 90 degrees around X so Y points to Z.
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    Matrix_Look_At(
                        m_Position,
                        m_Position + forward,
                        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
                    ) *
                    Matrix_Rotate_X(3.141592f / 2.0f) *
                    Matrix_Scale(0.08f, 1.25f, 0.08f);

  app.SetProjectileColor(m_Color);
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, model);
}

glm::vec4 Projectile::GetStartPoint() const {
  float speed = glm::length(m_Velocity);
  if (speed < 0.001f)
    return m_Position;
  glm::vec4 forward = m_Velocity / speed;
  return m_Position - forward * 2.5f;
}

glm::vec4 Projectile::GetEndPoint() const {
  float speed = glm::length(m_Velocity);
  if (speed < 0.001f)
    return m_Position;
  glm::vec4 forward = m_Velocity / speed;
  return m_Position + forward * 2.5f;
}
