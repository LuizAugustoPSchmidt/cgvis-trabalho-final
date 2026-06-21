#ifndef TIEDEFENDER_H
#define TIEDEFENDER_H

#include "objects/GameObject.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class TieDefender : public GameObject {
public:
  TieDefender(glm::vec4 position);
  virtual ~TieDefender() = default;

  virtual void Update(float deltaTime) override;
  void Update(float deltaTime, Application &app);
  void Shoot(Application &app);
  virtual void Render(Application &app) override;

  void SetTarget(glm::vec4 target) { m_Target = target; }
  virtual glm::vec4 GetPosition() const override { return m_Position; }
  glm::vec4 GetForward() const { return m_RotationMatrix[2]; }
  virtual float GetRadius() const override { return 1.5f; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Velocity = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  glm::vec4 m_Acceleration = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
  glm::vec4 m_Target = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 m_RotationMatrix = glm::mat4(1.0f);
  float m_ShootCooldown = 0.0f;
};

#endif
