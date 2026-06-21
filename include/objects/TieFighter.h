#ifndef TIEFIGHTER_H
#define TIEFIGHTER_H

#include "objects/GameObject.h"
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

class TieFighter : public GameObject {
public:
  TieFighter(glm::vec4 position);
  virtual ~TieFighter() = default;

  virtual void Update(float deltaTime) override;
  void Update(float deltaTime, Application &app);
  void Shoot(Application &app);
  virtual void Render(Application &app) override;

  void SetTarget(glm::vec4 target) { m_Target = target; }
  virtual glm::vec4 GetPosition() const override { return m_Position; }
  glm::vec4 GetForward() const {
    float s = glm::length(m_Velocity);
    return s > 0.001f ? m_Velocity / s : glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
  }
  virtual float GetRadius() const override { return 1.0f; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Velocity = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
  glm::vec4 m_Acceleration = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
  glm::vec4 m_Target = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 m_RotationMatrix = glm::mat4(1.0f);
  float m_ShootCooldown = 0.0f;
};

#endif
