#include "Asteroid.h"
#include "Application.h"
#include "ObjectIds.h"
#include "matrices.h"

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
    : GameObject("rock.001_rock.013", ASTEROID), m_Position(position),
      m_Scale(scale), m_Rotation(rotation) {}

void Asteroid::Update(float deltaTime) {
  m_CurveAngle += ASTEROID_CURVE_SPEED * m_Direction * deltaTime;
  float asteroidCurveTime =
      ASTEROID_CURVE_T_AMPLITUDE *
      (sin(m_CurveAngle - HALF_PI) + ASTEROID_CURVE_T_OFFSET);
  glm::vec4 asteroidPosition = CubicBezier(
      ASTEROID_CURVE_P0,
      ASTEROID_CURVE_P1,
      ASTEROID_CURVE_P2,
      ASTEROID_CURVE_P3,
      asteroidCurveTime
  );
  m_Position = asteroidPosition;
  m_ModelMatrix =
      Matrix_Translate(
          asteroidPosition.x, asteroidPosition.y, asteroidPosition.z
      ) *
      Matrix_Rotate_Y(m_CurveAngle);
}

void Asteroid::Render(Application &app) {
  app.DrawObject(m_ModelName.c_str(), m_ObjectId, m_ModelMatrix);
}
