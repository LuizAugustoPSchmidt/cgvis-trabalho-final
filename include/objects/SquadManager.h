#ifndef SQUAD_MANAGER_H
#define SQUAD_MANAGER_H

#include <array>
#include <memory>
#include <glm/geometric.hpp>
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

    void Coordinate(float deltaTime, const glm::vec4 &playerPos) {
        CleanupDeadMembers();

        // 1. Move squad center towards player (matching physics integration of TIEs)
        glm::vec4 targetDir = playerPos - m_Position;
        float dist = glm::length(targetDir);
        if (dist > 0.001f) {
            glm::vec4 acceleration = (targetDir / dist) * 10.0f; // ACCELERATION_MAX
            m_Velocity += acceleration * deltaTime;
            float speed = glm::length(m_Velocity);
            if (speed > 30.0f) { // SPEED_MAX
                m_Velocity = (m_Velocity / speed) * 30.0f;
            }
            m_Position += m_Velocity * deltaTime;
        }

        // 2. Set targets for TIEs in their YZ circle formation slots
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
