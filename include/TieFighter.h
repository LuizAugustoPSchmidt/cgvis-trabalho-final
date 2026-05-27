#ifndef TIEFIGHTER_H
#define TIEFIGHTER_H

#include "GameObject.h"
#include <glm/vec4.hpp>

class TieFighter : public GameObject {
public:
  TieFighter(glm::vec4 position);
  virtual ~TieFighter() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

private:
  glm::vec4 m_Position;
};

#endif
