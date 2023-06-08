/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>

#include <glm/vec2.hpp>

#include <RealEngine/user_input/Key.hpp>

namespace re {

/**
 * @brief Keeps track of mouse and keyboard input.
 */
class InputManager {
    friend class MainProgram;

public:
    InputManager() {}

    /**
     * @brief Should be called each step by the MainProgram
     */
    void update();

    /**
     * @brief Checks whether the key is currently pressed
     * @return For how many steps straight it has been down (0 = is not
     * currently down)
     */
    int isDown(Key keyID) const;

    /**
     * @brief Checks whther the key has just been pressed (= right before this step)
     * @return  How many times it has been pressed before this step (by how many
     * steps the mouse wheel was rotated) Usually returns 0 or 1.
     */
    int wasPressed(Key keyID) const;

    /**
     * @brief Checks whther the key has just been released (= right before this step)
     * @return 1 if the key has just been released, 0 otherwise
     */
    int wasReleased(Key keyID) const;

    /**
     * @brief Gets absolute position of the cursor.
     */
    const glm::ivec2& cursorAbs() const;

    /**
     * @brief Gets position of the cursor relative to previous step.
     */
    const glm::ivec2& cursorRel() const;

    // Setters
    void setCursor(const glm::ivec2& abs, const glm::ivec2& rel);

private:
    void press(Key keyID, int times = 1);
    void release(Key keyID);

    int                                  wasDown(Key keyID) const;
    mutable std::unordered_map<Key, int> m_stateMap;
    mutable std::unordered_map<Key, int> m_stateMapPrevious;
    glm::ivec2                           m_cursorAbs = glm::ivec2(0u, 0u);
    glm::ivec2                           m_cursorRel = glm::ivec2(0u, 0u);

    int m_keysHeld          = 0u;
    int m_keysHeldPrevious  = 0u;
    Key m_longestHeld       = Key::UnknownKey;
    int m_noKeyHeld         = 0u;
    int m_noKeyHeldPrevious = 0u;
};

} // namespace re