/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/user_input/InputManager.hpp>

#include <iostream>
#include <algorithm>

namespace RE {

InputManager::InputManager() {

}

void InputManager::update() {
    m_cursorRel = glm::ivec2(0, 0);
    release(Key::UMW);
    release(Key::DMW);
    release(Key::LMW);
    release(Key::RMW);
    int longest = 0;
    for (auto& i : m_stateMap) {
        if (i.first == Key::ANY_KEY) { continue; }
        if (i.second > 0) {
            m_stateMapPrevious[i.first] = ++i.second;
            if (i.second > longest) {
                longest = i.second;
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
    m_keysHeldPrevious = m_keysHeld;
}

int InputManager::isDown(RE::Key keyID) const {
    //First checking for special keys
    if (keyID == RE::Key::ANY_KEY) {
        return m_stateMap[m_longestHeld];
    } else if (keyID == RE::Key::NO_KEY) {
        return m_noKeyHeld;
    }

    auto it = m_stateMap.find(keyID);
    if (it != m_stateMap.end()) {
        return it->second;
    }
    return 0;
}

int InputManager::wasDown(RE::Key keyID) const {
    //First checking for special keys
    if (keyID == RE::Key::ANY_KEY) {
        return m_stateMapPrevious[m_longestHeld];
    } else if (keyID == RE::Key::NO_KEY) {
        return m_noKeyHeld;
    }

    auto it = m_stateMapPrevious.find(keyID);
    if (it != m_stateMapPrevious.end()) {
        return it->second;
    }
    return 0;
}

int InputManager::wasPressed(RE::Key keyID) const {
    //First checking for special keys
    if (keyID == RE::Key::ANY_KEY) {
        if (m_keysHeld > m_keysHeldPrevious) {
            return m_stateMap[m_longestHeld];
        } else {
            return 0;
        }
    } else if (keyID == RE::Key::NO_KEY) {
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

int InputManager::wasReleased(RE::Key keyID) const {
    //First checking for special keys
    if (keyID == RE::Key::ANY_KEY) {
        if (m_keysHeld < m_keysHeldPrevious) {
            return m_stateMap[m_longestHeld];
        } else {
            return 0;
        }
    } else if (keyID == RE::Key::NO_KEY) {
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

glm::ivec2 InputManager::getCursorAbs() const {
    return m_cursorAbs;
}

glm::ivec2 InputManager::getCursorRel() const {
    return m_cursorRel;
}

void InputManager::setCursor(const glm::ivec2& abs, const glm::ivec2& rel) {
    m_cursorAbs = abs;
    m_cursorRel = rel;
}

void InputManager::press(RE::Key keyID, int times/* = 1*/) {
    auto& it = m_stateMap[keyID];
    if (keyID == Key::ANY_KEY) {
        m_keysHeld += times;
    } else {
        if (m_keysHeld == 1) {
            m_longestHeld = keyID;
        }
        it += times;
    }
}

void InputManager::release(RE::Key keyID) {
    auto& it = m_stateMap[keyID];
    it = 0;
}

}