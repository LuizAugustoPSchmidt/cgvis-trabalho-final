#include "Application.h"
#include <algorithm>
#include <iostream>

bool Application::SphereSphere(
    glm::vec4 posA, float rA, glm::vec4 posB, float rB
) {
  glm::vec4 d = posA - posB;
  float dist2 = d.x * d.x + d.y * d.y + d.z * d.z;
  float rSum = rA + rB;
  return dist2 < rSum * rSum;
}

void Application::CheckCollisions() {
  // 1. Player vs Harmful (Enemies + Asteroids)
  std::vector<GameObject *> harmfulObjects;
  for (auto &a : m_Asteroids) harmfulObjects.push_back(a.get());
  for (auto &s : m_TieFighters) harmfulObjects.push_back(s.get());
  for (auto &s : m_TieDefenders) harmfulObjects.push_back(s.get());
  for (auto &s : m_TiePhantoms) harmfulObjects.push_back(s.get());

  std::vector<GameObject *> playerVec = {m_Player.get()};

  CheckCollisions(playerVec, harmfulObjects, [&](auto &p, auto &h) {
    m_GameOver = true;
  });

  if (m_GameOver)
    return;

  // 2. Enemies vs Asteroids
  auto enemyVsAsteroids = [&](auto &enemies) {
    CheckCollisions(enemies, m_Asteroids, [](auto &s, auto &a) {
#if !RELEASE
      std::cout << s->GetClassId() << " hit an asteroid" << std::endl;
#endif // !RELEASE
      s->Kill();
    });
  };
  enemyVsAsteroids(m_TieFighters);
  enemyVsAsteroids(m_TieDefenders);
  enemyVsAsteroids(m_TiePhantoms);

  // 3. Enemy vs Enemy
  std::vector<GameObject *> allEnemies;
  for (auto &s : m_TieFighters) allEnemies.push_back(s.get());
  for (auto &s : m_TieDefenders) allEnemies.push_back(s.get());
  for (auto &s : m_TiePhantoms) allEnemies.push_back(s.get());

  CheckCollisions(allEnemies, allEnemies, [](auto &s1, auto &s2) {
#if !RELEASE
    std::cout << s1->GetClassId() << " and " << s2->GetClassId() << " hit. Both dead" << std::endl;
#endif // !RELEASE
    s1->Kill();
    s2->Kill();
  });

  // 4. Cleanup dead ships
  auto cleanup = [](auto &vec) {
    vec.erase(
        std::remove_if(
            vec.begin(),
            vec.end(),
            [](const auto &s) { return s->IsDead(); }
        ),
        vec.end()
    );
  };
  cleanup(m_TieFighters);
  cleanup(m_TieDefenders);
  cleanup(m_TiePhantoms);

  // 5. Asteroid vs Asteroid
  CheckCollisions(m_Asteroids, m_Asteroids, [](auto &a1, auto &a2) {
    a1->ReverseDirection();
    a2->ReverseDirection();
  });
}
