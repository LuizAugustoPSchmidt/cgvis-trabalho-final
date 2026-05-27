#ifndef TIEDEFENDER_H
#define TIEDEFENDER_H

#include "GameObject.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class TieDefender : public GameObject {
public:
  TieDefender(glm::vec4 position);
  virtual ~TieDefender() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  void SetTarget(glm::vec4 target) { m_Target = target; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Target = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 m_RotationMatrix = glm::mat4(1.0f);
};

#endif
