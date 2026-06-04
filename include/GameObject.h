#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

class Application; // Forward declaration

class GameObject {
public:
  GameObject(std::string modelName, int objectId, std::string classId)
      : m_ModelName(modelName), m_ObjectId(objectId), m_ClassId(classId) {}
  virtual ~GameObject() = default;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(Application &app) = 0;

  std::string GetClassId() const { return m_ClassId; }

protected:
  std::string m_ModelName;
  int m_ObjectId;
  const std::string m_ClassId;
};

#endif // GAMEOBJECT_H
