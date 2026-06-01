#include "Player.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"
#include <glad/glad.h>

Player::Player()
    : GameObject("spaceship", SPACESHIP_MATERIAL),
      m_Position(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
      m_Forward(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)),
      m_Up(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) {
  m_Parts = {
      {"Cube", SPACESHIP_MATERIAL, true},
      {"Cube_motor_0", SPACESHIP_MOTOR, false},
      {"asas_CASCO_ESCURO_1_0", SPACESHIP_CASCO_ESCURO_1, false},
      {"asas_Casco_0", SPACESHIP_CASCO, false},
      {"asas_csaco_escuro_0", SPACESHIP_CSACO_ESCURO, false},
      {"asas_fundo_0", SPACESHIP_FUNDO, false},
      {"asas_Turbina_0", SPACESHIP_TURBINA, false},
      {"asas_FUNDO_2_0", SPACESHIP_FUNDO_2, false},
      {"asas_queimado_0", SPACESHIP_QUEIMADO, false},
      {"asas_motor_0", SPACESHIP_MOTOR, false},
      {"asas_pintura_0", SPACESHIP_PINTURA, false},
      {"asas_luz_turbina_0", SPACESHIP_LUZ_TURBINA, false},
      {"casco_queimado_0", SPACESHIP_QUEIMADO, true},
      {"casco_Casco_0", SPACESHIP_CASCO, true},
      {"casco_Cabine_0", SPACESHIP_CABINE, true},
      {"casco_ponta_0", SPACESHIP_PONTA, true},
      {"casco_pintura_0", SPACESHIP_PINTURA, true},
      {"casco_vidro_0", SPACESHIP_VIDRO, true},
      {"casco_fundo_0", SPACESHIP_FUNDO, true},
      {"robo_motor_0", SPACESHIP_MOTOR, false},
      {"robo_and_0", SPACESHIP_MATERIAL, false},
      {"robo_Material.001_0", SPACESHIP_MATERIAL_001, false},
      {"robo_pintura_0", SPACESHIP_PINTURA, false},
  };
}

void Player::Update(float deltaTime) {
  m_Forward = glm::vec4(
      -sin(m_Theta) * cos(m_Phi),
      sin(m_Phi),
      -cos(m_Theta) * cos(m_Phi),
      0.0f
  );
  m_Up = glm::vec4(
      sin(m_Theta) * sin(m_Phi),
      cos(m_Phi),
      cos(m_Theta) * sin(m_Phi),
      0.0f
  );
  m_Position += m_Forward * m_Speed * deltaTime;
}

void Player::Render(Application &app) {
  glm::mat4 model =
      Matrix_Translate(m_Position.x, m_Position.y, m_Position.z) *
      Matrix_Rotate_Y(m_Theta) *
      Matrix_Rotate_X(m_Phi) *
      Matrix_Scale(0.1f, 0.1f, 0.1f);

  for (const auto &part : m_Parts) {
    if (part.flip_normals) {
      glFrontFace(GL_CW);
    } else {
      glFrontFace(GL_CCW);
    }
    app.DrawObject(part.name.c_str(), part.object_id, model, part.flip_normals);
  }
  glFrontFace(GL_CCW); // Revert to default after the loop
}
