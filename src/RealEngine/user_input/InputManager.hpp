/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>

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
     * @brief Checks whether the key is currently pressed
     * @return For how many steps straight it has been down (0 = is not
     * currently down)
     */
    int isDown(Key key) const;

    /**
     * @brief Checks whther the key has just been pressed (= right before this step)
     * @return  How many times it has been pressed before this step (by how many
     * steps the mouse wheel was rotated) Usually returns 0 or 1.
     */
    int wasPressed(Key key) const;

    /**
     * @brief Checks whther the key has just been released (= right before this step)
     * @return 1 if the key has just been released, 0 otherwise
     */
    int wasReleased(Key key) const;

    /**
     * @brief Gets absolute position of the cursor.
     */
    const glm::ivec2& cursorAbs() const;

    /**
     * @brief Gets position of the cursor relative to previous step.
     */
    const glm::ivec2& cursorRel() const;

private:
    void step();
    void setCursor(const glm::ivec2& abs, const glm::ivec2& rel);
    void press(Key key, int times = 1);
    void release(Key key);

    int wasDown(Key key) const;

    /**
     * @brief Key (= index) => the number of steps that it has been pressed
     * continuously
     */
    using StateMap = std::array<int, static_cast<size_t>(Key::NumberOfKeys)>;
    mutable StateMap m_stateMap{};
    mutable StateMap m_stateMapPrev{};

    glm::ivec2 m_cursorAbs = glm::ivec2(0u, 0u);
    glm::ivec2 m_cursorRel = glm::ivec2(0u, 0u);
};

} // namespace re