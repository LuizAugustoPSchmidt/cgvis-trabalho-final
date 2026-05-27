#ifndef TIEPHANTOM_H
#define TIEPHANTOM_H

#include "GameObject.h"
#include <glm/vec4.hpp>
#include <vector>
#include <string>

struct PhantomPart {
    std::string name;
    int object_id;
};

class TiePhantom : public GameObject {
public:
  TiePhantom(glm::vec4 position);
  virtual ~TiePhantom() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

private:
  glm::vec4 m_Position;
  std::vector<PhantomPart> m_Parts;
};

#endif
