#include "Application.h"
#include <iostream>

bool Application::SpheresIntersect(
    glm::vec4 posA,
    float rA,
    glm::vec4 posB,
    float rB
) {
  glm::vec4 d = posA - posB;
  float dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
  float rSum = rA + rB;
  return dist2 < rSum * rSum;
}

void Application::CheckCollisions() {
  // 1. Player vs Harmful (Enemies + Asteroids)
  std::vector<GameObject *> harmfulObjects = GetHarmfulObjects();
  std::vector<GameObject *> playerVec = {m_Player.get()};
  std::vector<GameObject *> allEnemies = GetAllEnemies();

  CheckCollisions(playerVec, harmfulObjects, [&](auto &p, auto &h) {
    m_Player->Kill();
  });

  // New Phase 2: Player vs Projectiles
  CheckCollisions(playerVec, m_Projectiles, [&](auto &p, auto &proj) {
    if (proj->IsEnemy()) {
      proj->Kill();
      m_Player->Kill();
    }
  });

  if (m_Player->IsDead()) {
    m_GameOver = true;
    return;
  }

  // 2. Projectiles vs Enemies
  CheckCollisions(m_Projectiles, allEnemies, [](auto &proj, auto &enemy) {
    if (!proj->IsEnemy()) {
#if !RELEASE
      std::cout << enemy->GetClassId() << " hit by " << proj->GetClassId()
                << std::endl;
#endif // !RELEASE
      proj->Kill();
      enemy->TakeDamage(1);
    }
  });

  // 3. All Enemies vs Asteroids
  CheckCollisions(allEnemies, m_Asteroids, [](auto &s, auto &a) {
#if !RELEASE
    std::cout << s->GetClassId() << " hit an asteroid" << std::endl;
#endif // !RELEASE
    s->Kill();
  });

  // 4. Enemy vs Enemy
  CheckCollisions(allEnemies, allEnemies, [](auto &s1, auto &s2) {
#if !RELEASE
    std::cout << s1->GetClassId() << " (Squad " << s1->GetSquadId() << ") and "
              << s2->GetClassId() << " (Squad " << s2->GetSquadId() << ") hit. Both dead" << std::endl;
#endif // !RELEASE
    s1->Kill();
    s2->Kill();
  });

  // 5. Asteroid vs Asteroid
  CheckCollisions(m_Asteroids, m_Asteroids, [](auto &a1, auto &a2) {
    a1->ReverseDirection();
    a2->ReverseDirection();
  });
}
