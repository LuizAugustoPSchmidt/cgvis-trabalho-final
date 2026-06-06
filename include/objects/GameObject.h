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
  bool IsDead() const { return m_IsDead; }
  void Kill() { m_IsDead = true; }

protected:
  std::string m_ModelName;
  int m_ObjectId;
  const std::string m_ClassId;
  bool m_IsDead = false;
};

#endif // GAMEOBJECT_H
