#ifndef TIEDEFENDER_H
#define TIEDEFENDER_H

#include "GameObject.h"
#include <glm/vec4.hpp>

class TieDefender : public GameObject {
public:
  TieDefender(glm::vec4 position);
  virtual ~TieDefender() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

private:
  glm::vec4 m_Position;
};

#endif
