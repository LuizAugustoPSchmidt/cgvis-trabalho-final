#ifndef SQUAD_MANAGER_H
#define SQUAD_MANAGER_H

#include <array>
#include <memory>
#include <glm/geometric.hpp>
#include <vector>
#include "matrices.h"
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

    glm::vec4 GetLocalOffset(int i) const {
        float rBase = 12.0f;
        float rMid = 6.0f;
        float dZ = 10.0f;

        if (i < 7) {
            // Base Layer (Layer 0, Z = -10.0f)
            if (i == 0) return glm::vec4(0.0f, 0.0f, -dZ, 0.0f); // Center
            float angle = (2.0f * 3.14159265f * (i - 1)) / 6.0f;
            return glm::vec4(rBase * cos(angle), rBase * sin(angle), -dZ, 0.0f);
        } else if (i < 11) {
            // Middle Layer (Layer 1, Z = 0.0f)
            int midIdx = i - 7;
            if (midIdx == 0) return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Center
            float angle = (2.0f * 3.14159265f * (midIdx - 1)) / 3.0f;
            return glm::vec4(rMid * cos(angle), rMid * sin(angle), 0.0f, 0.0f);
        } else {
            // Apex (Layer 2, Z = 10.0f)
            return glm::vec4(0.0f, 0.0f, dZ, 0.0f);
        }
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

        // Update rotation matrix to point along velocity vector
        if (speed > 0.001f) {
            glm::vec4 forward = m_Velocity / speed;
            m_RotationMatrix = Matrix_Look_At(
                m_Position,
                m_Position + forward,
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)
            );
        }

        // 3. Set targets for TIEs in their YZ circle formation slots
        for (int i = 0; i < SquadTraits<T>::MAX_CAPACITY; ++i) {
            T* tie = m_Ties[i];
            if (!tie) continue;

            glm::vec4 localOffset = GetLocalOffset(i);
            glm::vec4 rotatedOffset = m_RotationMatrix * localOffset;
            tie->SetTarget(m_Position + rotatedOffset);
        }
    }

private:
    glm::vec4 m_Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 m_Velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::mat4 m_RotationMatrix = glm::mat4(1.0f);
    std::array<T*, SquadTraits<T>::MAX_CAPACITY> m_Ties;
    int m_Size;
};

#endif // SQUAD_MANAGER_H
