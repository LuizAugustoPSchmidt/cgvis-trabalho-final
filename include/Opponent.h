#ifndef OPPONENT_H
#define OPPONENT_H

#include "GameObject.h"
#include <glm/vec4.hpp>

class Opponent : public GameObject {
public:
  Opponent(glm::vec4 position);
  virtual ~Opponent() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

private:
  glm::vec4 m_Position;
};

#endif // OPPONENT_H
