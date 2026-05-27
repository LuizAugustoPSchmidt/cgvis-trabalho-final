#include "TiePhantom.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

TiePhantom::TiePhantom(glm::vec4 position)
    : GameObject("tiephantom_mat0", TIE_PHANTOM_HULL), m_Position(position) {
  m_Parts = {
      {"tiephantom_mat0", TIE_PHANTOM_HULL},
      {"tiephantom_mat1", TIE_PHANTOM_WINGS},
      {"tiephantom_mat2", TIE_PHANTOM_WINGS},
      {"tiephantom_mat3", TIE_PHANTOM_WINGS},
      {"tiephantom_mat4", TIE_PHANTOM_WINGS},
      {"tiephantom_mat5", TIE_PHANTOM_WINGS},
  };
}

void TiePhantom::Update(float deltaTime) {
  // Movement logic
}

void TiePhantom::Render(Application &app) {
  // Tie Phantom: Size ~1.0, already mostly centered.
  // Scale by 2.0 to match TIE Fighter size.
  // Rotate 90 degrees around X to point forward.
  glm::mat4 model = Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
                    Matrix_Scale(2.0f, 2.0f, 2.0f) *
                    Matrix_Rotate_X(3 * 3.141592f / 2.0f);
  for (const auto &part : m_Parts) {
    app.DrawObject(part.name.c_str(), part.object_id, model);
  }
}
