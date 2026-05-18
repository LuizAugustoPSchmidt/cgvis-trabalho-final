#include "Opponent.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

Opponent::Opponent(glm::vec4 position) : m_Position(position) {}

void Opponent::Update(float deltaTime) {
  // Bezier curve movement will be implemented here
}

void Opponent::Render(Application &app) {
  // Using asteroid model as placeholder for opponent for now, or just sphere
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    Matrix_Scale(0.5f, 0.5f, 0.5f);
  app.DrawObject("the_sphere", SPHERE, model);
}
