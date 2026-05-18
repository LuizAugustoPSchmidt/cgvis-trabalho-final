#include "Asteroid.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

Asteroid::Asteroid(glm::vec4 position, glm::vec4 scale, glm::mat4 rotation)
    : m_Position(position), m_Scale(scale), m_Rotation(rotation) {}

void Asteroid::Update(float deltaTime) {
  // Could add some rotation over time here
}

void Asteroid::Render(Application &app) {
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    m_Rotation * Matrix_Scale(m_Scale.x, m_Scale.y, m_Scale.z);
  app.DrawObject("rock.001_rock.013", ASTEROID, model);
}
