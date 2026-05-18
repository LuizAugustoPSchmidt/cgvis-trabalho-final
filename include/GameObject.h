#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

class Application; // Forward declaration

class GameObject {
public:
  virtual ~GameObject() = default;
  virtual void Update(float deltaTime) = 0;
  virtual void Render(Application &app) = 0;
};

#endif // GAMEOBJECT_H
