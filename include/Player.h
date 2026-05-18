#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <glm/vec4.hpp>
#include <vector>
#include <string>

struct SpaceshipPart {
  std::string name;
  int object_id;
  bool flip_normals;
};

class Player : public GameObject {
public:
  Player();
  virtual ~Player() = default;

  virtual void Update(float deltaTime) override;
  virtual void Render(Application &app) override;

  glm::vec4 GetPosition() const { return m_Position; }
  glm::vec4 GetForward() const { return m_Forward; }
  glm::vec4 GetUp() const { return m_Up; }

  void SetPosition(glm::vec4 pos) { m_Position = pos; }

private:
  glm::vec4 m_Position;
  glm::vec4 m_Forward;
  glm::vec4 m_Up;

  std::vector<SpaceshipPart> m_Parts;
};

#endif // PLAYER_H
