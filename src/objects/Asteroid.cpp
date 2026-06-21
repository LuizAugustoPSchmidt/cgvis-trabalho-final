#include "objects/Asteroid.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"
#include <cstdlib>

constexpr float PI = 3.141592f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = PI / 2.0f;
constexpr float ASTEROID_CURVE_SPEED = 0.45f;
constexpr float ASTEROID_CURVE_T_AMPLITUDE = 0.5f;
constexpr float ASTEROID_CURVE_T_OFFSET = 1.0f;

constexpr glm::vec4 ASTEROID_CURVE_P0 = glm::vec4(15.0f, 2.0f, -20.0f, 1.0f);
constexpr glm::vec4 ASTEROID_CURVE_P1 = glm::vec4(28.0f, 14.0f, -34.0f, 1.0f);
constexpr glm::vec4 ASTEROID_CURVE_P2 = glm::vec4(-18.0f, 18.0f, -48.0f, 1.0f);
constexpr glm::vec4 ASTEROID_CURVE_P3 = glm::vec4(-25.0f, 10.0f, -30.0f, 1.0f);

static glm::vec4 CubicBezier(
    const glm::vec4 &p0,
    const glm::vec4 &p1,
    const glm::vec4 &p2,
    const glm::vec4 &p3,
    float t
) {
  float u = 1.0f - t;
  return (u * u * u) * p0 + (3.0f * u * u * t) * p1 + (3.0f * u * t * t) * p2 +
         (t * t * t) * p3;
}

Asteroid::Asteroid(glm::vec4 position, glm::vec4 scale, glm::mat4 rotation)
    : GameObject("rock.001_rock.013", ASTEROID, "asteroid"),
      m_Position(position), m_Scale(scale), m_Rotation(rotation) {
  // Generate random Bezier path around position (Curve 1)
  m_P0 = position;
  m_P1 = position + glm::vec4(float(rand() % 40 - 20), float(rand() % 40 - 20), float(rand() % 40 - 20), 0.0f);
  m_P2 = position + glm::vec4(float(rand() % 40 - 20), float(rand() % 40 - 20), float(rand() % 40 - 20), 0.0f);
  m_P3 = position + glm::vec4(float(rand() % 40 - 20), float(rand() % 40 - 20), float(rand() % 40 - 20), 0.0f);

  // Constraint-based control points for C1 continuity (velocity matching)
  m_P1_c2 = 2.0f * m_P3 - m_P2;
  m_P2_c2 = 2.0f * m_P0 - m_P1;

  // Randomize initial curve angle and speed/direction
  m_CurveAngle = static_cast<float>(rand() % 100) / 100.0f * TWO_PI;
  m_Direction = (rand() % 2 == 0) ? 1.0f : -1.0f;

  // Randomize initial path progression and curve choice
  m_T = static_cast<float>(rand() % 100) / 100.0f;
  m_OnCurve1 = (rand() % 2 == 0);
}

void Asteroid::Update(float deltaTime) {
  // Keep rotating the asteroid smoothly
  m_CurveAngle += ASTEROID_CURVE_SPEED * m_Direction * deltaTime;

  // Advance along the Bezier curve loop (approx. 6.6s per curve)
  m_T += 0.15f * deltaTime;
  if (m_T >= 1.0f) {
    m_T = fmod(m_T, 1.0f);
    m_OnCurve1 = !m_OnCurve1;
  }

  glm::vec4 asteroidPosition;
  if (m_OnCurve1) {
    asteroidPosition = CubicBezier(m_P0, m_P1, m_P2, m_P3, m_T);
  } else {
    asteroidPosition = CubicBezier(m_P3, m_P1_c2, m_P2_c2, m_P0, m_T);
  }

  m_Position = asteroidPosition;
  m_ModelMatrix = Matrix_Translate(
                      asteroidPosition.x,
                      asteroidPosition.y,
                      asteroidPosition.z
                  ) *
                  m_Rotation *
                  Matrix_Rotate_Y(m_CurveAngle) *
                  Matrix_Scale(m_Scale.x, m_Scale.y, m_Scale.z);
}

void Asteroid::Render(Application &app) {
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, m_ModelMatrix);
}
