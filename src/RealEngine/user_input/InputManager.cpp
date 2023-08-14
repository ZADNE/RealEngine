/*!
 *  @author    Dubsky Tomas
 */
#include <algorithm>
#include <iostream>

#include <RealEngine/user_input/InputManager.hpp>

namespace re {

void InputManager::update() {
    m_cursorRel = glm::ivec2(0, 0);
    release(Key::UMW);
    release(Key::DMW);
    release(Key::LMW);
    release(Key::RMW);
    int longest = 0;
    for (auto& i : m_stateMap) {
        if (i.first == Key::AnyKey) {
            continue;
        }
        if (i.second > 0) {
            m_stateMapPrevious[i.first] = ++i.second;
            if (i.second > longest) {
                longest       = i.second;
                m_longestHeld = i.first;
            }
        } else {
            m_stateMapPrevious[i.first] = i.second;
        }
    }

    ++m_noKeyHeld;
    if (m_keysHeld > 0) {
        m_noKeyHeld = 0;
    }

    m_noKeyHeldPrevious = m_noKeyHeld;
    m_keysHeldPrevious  = m_keysHeld;
}

int InputManager::isDown(Key keyID) const {
    // First check for special keys
    if (keyID == Key::AnyKey) {
        return m_stateMap[m_longestHeld];
    } else if (keyID == Key::NoKey) {
        return m_noKeyHeld;
    }

    auto it = m_stateMap.find(keyID);
    if (it != m_stateMap.end()) {
        return it->second;
    }
    return 0;
}

int InputManager::wasDown(Key keyID) const {
    // First check for special keys
    if (keyID == Key::AnyKey) {
        return m_stateMapPrevious[m_longestHeld];
    } else if (keyID == Key::NoKey) {
        return m_noKeyHeld;
    }

    auto it = m_stateMapPrevious.find(keyID);
    if (it != m_stateMapPrevious.end()) {
        return it->second;
    }
    return 0;
}

int InputManager::wasPressed(Key keyID) const {
    // First check for special keys
    if (keyID == Key::AnyKey) {
        if (m_keysHeld > m_keysHeldPrevious) {
            return m_stateMap[m_longestHeld];
        } else {
            return 0;
        }
    } else if (keyID == Key::NoKey) {
        if (m_keysHeld < m_keysHeldPrevious && m_keysHeld == 0) {
            return 1;
        } else {
            return 0;
        }
    }

    if (isDown(keyID) > wasDown(keyID)) {
        return m_stateMap[keyID];
    }
    return 0;
}

int InputManager::wasReleased(Key keyID) const {
    // First check for special keys
    if (keyID == Key::AnyKey) {
        if (m_keysHeld < m_keysHeldPrevious) {
            return m_stateMap[m_longestHeld];
        } else {
            return 0;
        }
    } else if (keyID == Key::NoKey) {
        if (m_keysHeld > m_keysHeldPrevious && m_keysHeldPrevious == 0) {
            return m_noKeyHeld + 1;
        } else {
            return 0;
        }
    }

    if (wasDown(keyID) > isDown(keyID)) {
        return m_stateMapPrevious[keyID];
    }
    return 0;
}

const glm::ivec2& InputManager::cursorAbs() const {
    return m_cursorAbs;
}

const glm::ivec2& InputManager::cursorRel() const {
    return m_cursorRel;
}

void InputManager::setCursor(const glm::ivec2& abs, const glm::ivec2& rel) {
    m_cursorAbs = abs;
    m_cursorRel = rel;
}

void InputManager::press(Key keyID, int times /* = 1*/) {
    if (keyID == Key::AnyKey) {
        m_keysHeld += times;
    } else {
        if (m_keysHeld == 1) {
            m_longestHeld = keyID;
        }
        m_stateMap[keyID] += times;
    }
}

void InputManager::release(Key keyID) {
    m_stateMap[keyID] = 0;
}

} // namespace re