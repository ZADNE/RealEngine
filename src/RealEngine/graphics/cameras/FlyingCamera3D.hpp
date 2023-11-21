/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace re {

/**
 * @brief Is a moving 3D camera with pitch/yaw/roll rotations
 */
class FlyingCamera3D {
public:
    FlyingCamera3D(glm::vec3 pos, glm::vec3 yawPitchRoll);

    /**
     * @brief Adds given angles (in radians) to the current ones
     */
    void rotate(glm::vec3 yawPitchRoll);

    /**
     * @brief Moves based on current rotation
     */
    void move(glm::vec3 rightUpBack);

    /**
     * @brief Gets view matrix of the camera
     */
    const glm::mat4& viewMat() const { return m_viewMat; }

    /**
     * @brief Calculates view matrix that the camera would have if it has
     * rotated and moved
     * @details This does not change the state camera.
     */
    glm::mat4 calculateRelativeViewMat(
        glm::vec3 yawPitchRoll, glm::vec3 rightUpBack
    ) const;

    /**
     * @brief Gets position of the camera
     */
    glm::vec3 pos() const { return m_pos; }

    /**
     * @brief Gets quaternion representation of the camera's direction
     */
    glm::quat dir() const { return m_dirQuat; }

private:
    glm::mat4 calculateViewMat(glm::quat dirQuat, glm::vec3 pos) const;

    glm::mat4 m_viewMat;
    glm::vec3 m_pos;
    glm::quat m_dirQuat{};
};

} // namespace re
