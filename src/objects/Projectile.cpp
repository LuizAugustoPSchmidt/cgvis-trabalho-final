#include "objects/Projectile.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

Projectile::Projectile(glm::vec4 position, glm::vec4 velocity)
    : GameObject("the_sphere", LASER_BOLT, "projectile"), m_Position(position),
      m_Velocity(velocity) {}

void Projectile::Update(float deltaTime) {
  m_Position += m_Velocity * deltaTime;
  m_TimeAlive += deltaTime;
  if (m_TimeAlive >= MAX_LIFETIME)
    Kill();
}

void Projectile::Render(Application &app) {
  glm::vec4 tip = m_Position + m_Velocity * 0.1f;
  glm::mat4 model =
      Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
      Matrix_Look_At(m_Position, tip, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) *
      Matrix_Scale(0.08f, 0.08f, 2.5f);
  app.DrawObject("the_sphere", LASER_BOLT, model);
}
