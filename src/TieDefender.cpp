#include "TieDefender.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

TieDefender::TieDefender(glm::vec4 position)
    : GameObject("tiedefender_obj1", TIE_DEFENDER), m_Position(position) {}

void TieDefender::Update(float deltaTime) {
  m_RotationMatrix = Matrix_Look_At(m_Position, m_Target, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

void TieDefender::Render(Application &app) {
  // Tie Defender: Size ~3000, Center ~ (0, 1120, 160)
  // Scale by 0.0006 to match TIE Fighter size.
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    m_RotationMatrix *
                    Matrix_Scale(0.0006f, 0.0006f, 0.0006f) *
                    Matrix_Translate(0.0f, -1120.0f, -160.0f);
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, model);
}
