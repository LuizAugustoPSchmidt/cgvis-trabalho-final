#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <glm/vec4.hpp>

class Application; // Forward declaration

class GameObject {
public:
  GameObject(std::string modelName, int objectId, std::string classId)
      : m_ModelName(modelName), m_ObjectId(objectId), m_ClassId(classId) {}
  virtual ~GameObject() = default;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(Application &app) = 0;

  virtual glm::vec4 GetPosition() const = 0;
  virtual float GetRadius() const = 0;

  std::string GetClassId() const { return m_ClassId; }
  bool IsDead() const { return m_Health <= 0; }
  void Kill() { m_Health = 0; }
  int GetHealth() const { return m_Health; }
  void TakeDamage(int amount) { m_Health -= amount; }
  int GetSquadId() const { return m_SquadId; }
  void SetSquadId(int id) { m_SquadId = id; }

protected:
  std::string m_ModelName;
  int m_ObjectId;
  const std::string m_ClassId;
  int m_Health = 3;
  int m_SquadId = -1;
};

#endif // GAMEOBJECT_H
