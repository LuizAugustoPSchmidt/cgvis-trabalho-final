#ifndef TIEFIGHTER_H
#define TIEFIGHTER_H

#include "GameObject.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class TieFighter : public GameObject {
public:
  TieFighter(glm::vec4 position);
  virtual ~TieFighter() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  void SetTarget(glm::vec4 target) { m_Target = target; }
  glm::vec4 GetPosition() const { return m_Position; }
  float GetRadius() const { return 1.0f; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Target = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 m_RotationMatrix = glm::mat4(1.0f);
};

#endif
