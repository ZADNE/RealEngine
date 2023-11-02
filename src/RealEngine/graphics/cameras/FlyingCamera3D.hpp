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
    FlyingCamera3D(const glm::vec3& pos, const glm::vec3& yawPitchRoll);

    /**
     * @brief Adds given angles (in radians) to the current ones
     */
    void rotate(const glm::vec3& yawPitchRoll);

    /**
     * @brief Moves based on current rotation
     */
    void move(const glm::vec3& leftUpForward);

    /**
     * @brief Gets the projection matrix of the view
     */
    const glm::mat4& viewMat() const;

    /**
     * @brief Gets position of the camera
     */
    const glm::vec3& pos() const { return m_pos; }

private:
    void      recalculateViewMat();
    glm::mat4 m_viewMat;

    glm::vec3 m_pos;
    glm::quat m_dirQuat{};
};

} // namespace re
