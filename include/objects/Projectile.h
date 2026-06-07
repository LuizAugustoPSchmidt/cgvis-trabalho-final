#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "objects/GameObject.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Projectile : public GameObject {
public:
  Projectile(glm::vec4 position, glm::vec4 velocity, glm::vec3 color);
  virtual ~Projectile() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  virtual glm::vec4 GetPosition() const override { return m_Position; }
  virtual float GetRadius() const override { return 0.3f; }
  glm::vec3 GetColor() const { return m_Color; }

  glm::vec4 GetStartPoint() const;
  glm::vec4 GetEndPoint() const;

private:
  glm::vec4 m_Position;
  glm::vec4 m_Velocity;
  glm::vec3 m_Color;
  float m_TimeAlive = 0.0f;
  static constexpr float MAX_LIFETIME = 3.0f;
};

#endif // PROJECTILE_H
