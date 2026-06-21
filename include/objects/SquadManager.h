#ifndef SQUAD_MANAGER_H
#define SQUAD_MANAGER_H

#include <array>
#include <memory>
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

    void Coordinate(float deltaTime) {
        CleanupDeadMembers();
        // Coordination logic will be implemented here
    }

private:
    std::array<T*, SquadTraits<T>::MAX_CAPACITY> m_Ties;
    int m_Size;
};

#endif // SQUAD_MANAGER_H
