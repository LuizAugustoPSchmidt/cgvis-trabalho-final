#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>

class Application; // Forward declaration

class GameObject {
public:
  GameObject(std::string modelName, int objectId)
      : m_ModelName(modelName), m_ObjectId(objectId) {}
  virtual ~GameObject() = default;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(Application &app) = 0;

protected:
  std::string m_ModelName;
  int m_ObjectId;
};

#endif // GAMEOBJECT_H
