#ifndef TIEPHANTOM_H
#define TIEPHANTOM_H

#include "GameObject.h"
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
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

  void SetTarget(glm::vec4 target) { m_Target = target; }
  glm::vec4 GetPosition() const { return m_Position; }
  float GetRadius() const { return 2.0f; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Target = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  glm::mat4 m_RotationMatrix = glm::mat4(1.0f);
  std::vector<PhantomPart> m_Parts;
};

#endif
