#include "TieFighter.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

TieFighter::TieFighter(glm::vec4 position)
    : GameObject("tiefighter_TIE_FIghter_Sphere.004", TIE_FIGHTER), m_Position(position) {}

void TieFighter::Update(float deltaTime) {
  m_RotationMatrix = Matrix_Look_At(m_Position, m_Target, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

void TieFighter::Render(Application &app) {
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    m_RotationMatrix *
                    Matrix_Scale(0.5f, 0.5f, 0.5f);
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, model);
}
