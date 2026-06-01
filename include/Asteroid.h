#ifndef ASTEROID_H
#define ASTEROID_H

#include "GameObject.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Asteroid : public GameObject {
public:
  Asteroid(glm::vec4 position, glm::vec4 scale = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::mat4 rotation = glm::mat4(1.0f));
  virtual ~Asteroid() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  float GetCurveAngle() const { return m_CurveAngle; }
  glm::vec4 GetPosition() const { return m_Position; }
  float GetRadius() const { return 2.0f; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Scale;
  glm::mat4 m_Rotation;
  float m_CurveAngle = 0.0f;
  glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
};

#endif // ASTEROID_H
