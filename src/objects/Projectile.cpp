#include "objects/Projectile.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

Projectile::Projectile(glm::vec4 position, glm::vec4 velocity)
    : GameObject("the_sphere", LASER_BOLT, "projectile"),
      m_Position(position),
      m_Velocity(velocity) {}

void Projectile::Update(float deltaTime) {
  m_Position += m_Velocity * deltaTime;
  m_TimeAlive += deltaTime;
  if (m_TimeAlive >= MAX_LIFETIME)
    Kill();
}

void Projectile::Render(Application &app) {
  glm::mat4 model =
      Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
      Matrix_Scale(0.15f, 0.15f, 0.15f);
  app.DrawObject("the_sphere", LASER_BOLT, model);
}
