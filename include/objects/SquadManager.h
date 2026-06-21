#ifndef SQUAD_MANAGER_H
#define SQUAD_MANAGER_H

#include <array>
#include <memory>
#include <glm/geometric.hpp>
#include <vector>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// Include constants to get squad sizes
#include "Application.constants.h"

// Forward declarations
class TieFighter;
class TieDefender;
class TiePhantom;

template <typename T>
struct SquadTraits;

template <>
struct SquadTraits<TieFighter> {
    static constexpr int MAX_CAPACITY = TIE_FIGHTER_SQUADRON_SIZE;
};

template <>
struct SquadTraits<TieDefender> {
    static constexpr int MAX_CAPACITY = TIE_DEFENDER_SQUADRON_SIZE;
};

template <>
struct SquadTraits<TiePhantom> {
    static constexpr int MAX_CAPACITY = TIE_PHANTOM_SQUADRON_SIZE;
};

// TODO: In the future, we should move the TIEs' ownership (std::unique_ptr) into the SquadManager class
template <typename T>
class SquadManager {
public:
    SquadManager() : m_Size(0) {
        m_Ties.fill(nullptr);
    }

    ~SquadManager() = default;

    void SetPosition(const glm::vec4 &position) { m_Position = position; }
    glm::vec4 GetPosition() const { return m_Position; }

    // Bind a TIE to this squad
    bool AddMember(T* tie) {
        if (m_Size >= SquadTraits<T>::MAX_CAPACITY) {
            return false;
        }
        m_Ties[m_Size++] = tie;
        return true;
    }

    // Nullify destroyed members
    void CleanupDeadMembers() {
        for (int i = 0; i < m_Size; ++i) {
            if (m_Ties[i] && m_Ties[i]->IsDead()) {
                m_Ties[i] = nullptr;
            }
        }
    }

    int GetSize() const { return m_Size; }
    T* GetTie(int index) const {
        if (index < 0 || index >= m_Size) return nullptr;
        return m_Ties[index];
    }

    void Coordinate(float deltaTime, const glm::vec4 &playerPos, const std::vector<glm::vec4> &otherSquads) {
        CleanupDeadMembers();

        // 1. Calculate attraction to player
        glm::vec4 attraction = glm::vec4(0.0f);
        glm::vec4 playerDir = playerPos - m_Position;
        float playerDist = glm::length(playerDir);
        if (playerDist > 0.001f) {
            attraction = (playerDir / playerDist) * SQUAD_ATTRACTION_WEIGHT;
        }

        // 2. Calculate repulsion from other squads
        glm::vec4 repulsion = glm::vec4(0.0f);
        for (const auto &otherPos : otherSquads) {
            glm::vec4 diff = m_Position - otherPos;
            float dist = glm::length(diff);
            if (dist > 0.001f && dist < SQUAD_REPULSION_THRESHOLD) {
                repulsion += (diff / dist) * (SQUAD_REPULSION_WEIGHT / dist);
            }
        }

        glm::vec4 acceleration = attraction + repulsion;
        float accLen = glm::length(acceleration);
        if (accLen > 10.0f) { // ACCELERATION_MAX
            acceleration = (acceleration / accLen) * 10.0f;
        }

        m_Velocity += acceleration * deltaTime;
        float speed = glm::length(m_Velocity);
        if (speed > 30.0f) { // SPEED_MAX
            m_Velocity = (m_Velocity / speed) * 30.0f;
        }
        m_Position += m_Velocity * deltaTime;

        // 3. Set targets for TIEs in their YZ circle formation slots
        for (int i = 0; i < SquadTraits<T>::MAX_CAPACITY; ++i) {
            T* tie = m_Ties[i];
            if (!tie) continue;

            float angle = (2.0f * 3.14159265f * i) / SquadTraits<T>::MAX_CAPACITY;
            glm::vec4 offset = glm::vec4(
                0.0f,
                SQUAD_SPAWN_RADIUS * sin(angle),
                SQUAD_SPAWN_RADIUS * cos(angle),
                0.0f
            );
            tie->SetTarget(m_Position + offset);
        }
    }

private:
    glm::vec4 m_Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 m_Velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    std::array<T*, SquadTraits<T>::MAX_CAPACITY> m_Ties;
    int m_Size;
};

#endif // SQUAD_MANAGER_H
