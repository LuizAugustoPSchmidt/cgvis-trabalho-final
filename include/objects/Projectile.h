#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "objects/GameObject.h"
#include <glm/vec4.hpp>

class Projectile : public GameObject {
public:
  Projectile(glm::vec4 position, glm::vec4 velocity);
  virtual ~Projectile() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  virtual glm::vec4 GetPosition() const override { return m_Position; }
  virtual float GetRadius() const override { return 0.3f; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Velocity;
  float m_TimeAlive = 0.0f;
  static constexpr float MAX_LIFETIME = 3.0f;
};

#endif // PROJECTILE_H
