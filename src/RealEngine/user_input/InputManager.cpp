/*!
 *  @author    Dubsky Tomas
 */
#include <algorithm>
#include <iostream>

#include <RealEngine/user_input/InputManager.hpp>

namespace re {

namespace {

constexpr int cast(Key key) {
    return static_cast<std::underlying_type_t<Key>>(key);
}

} // namespace

int InputManager::isDown(Key key) const {
    return m_stateMap[cast(key)];
}

int InputManager::wasDown(Key key) const {
    return m_stateMapPrev[cast(key)];
}

int InputManager::wasPressed(Key key) const {
    return (isDown(key) > wasDown(key)) ? m_stateMap[cast(key)] : 0;
}

int InputManager::wasReleased(Key key) const {
    return (wasDown(key) > isDown(key)) ? m_stateMapPrev[cast(key)] : 0;
}

const glm::ivec2& InputManager::cursorAbs() const {
    return m_cursorAbs;
}

const glm::ivec2& InputManager::cursorRel() const {
    return m_cursorRel;
}

void InputManager::step() {
    m_cursorRel = glm::ivec2(0, 0);
    release(Key::UMW);
    release(Key::DMW);
    release(Key::LMW);
    release(Key::RMW);

    int longestHeld = 0;
    for (int i = 0; i < cast(Key::FirstSpecialKey); ++i) {
        int& state        = m_stateMap[i];
        m_stateMapPrev[i] = (state > 0) ? ++state : state;
        longestHeld       = std::max(longestHeld, state);
    }

    m_stateMapPrev[cast(Key::AnyKey)] = m_stateMap[cast(Key::AnyKey)];
    m_stateMap[cast(Key::AnyKey)]     = longestHeld > 0;
    m_stateMapPrev[cast(Key::NoKey)]  = m_stateMap[cast(Key::NoKey)];
    m_stateMap[cast(Key::NoKey)]      = longestHeld == 0;
}

void InputManager::setCursor(const glm::ivec2& abs, const glm::ivec2& rel) {
    m_cursorAbs = abs;
    m_cursorRel = rel;
}

void InputManager::press(Key key, int times /* = 1*/) {
    assert(!isSpecialKey(key) || key == Key::UnknownKey);
    m_stateMap[cast(key)] += times;
}

void InputManager::release(Key key) {
    assert(!isSpecialKey(key) || key == Key::UnknownKey);
    m_stateMap[cast(key)] = 0;
}

} // namespace re