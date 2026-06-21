#ifndef ASTEROID_H
#define ASTEROID_H

#include "objects/GameObject.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Asteroid : public GameObject {
public:
  Asteroid(glm::vec4 position, glm::vec4 scale = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), glm::mat4 rotation = glm::mat4(1.0f));
  virtual ~Asteroid() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  float GetCurveAngle() const { return m_CurveAngle; }
  virtual glm::vec4 GetPosition() const override { return m_Position; }
  virtual float GetRadius() const override { return 2.0f * m_Scale.x; }
  void ReverseDirection() { m_Direction = -m_Direction; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Scale;
  glm::mat4 m_Rotation;
  float m_CurveAngle = 0.0f;
  float m_Direction = 1.0f;
  glm::mat4 m_ModelMatrix = glm::mat4(1.0f);

  // Unique Bezier control points for this asteroid
  glm::vec4 m_P0;
  glm::vec4 m_P1;
  glm::vec4 m_P2;
  glm::vec4 m_P3;

  // Control points for the second Bezier curve (starts at m_P3, ends at m_P0)
  glm::vec4 m_P1_c2;
  glm::vec4 m_P2_c2;

  bool m_OnCurve1 = true;
  float m_T = 0.0f;
};

#endif // ASTEROID_H
