/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace RE {

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
    void move(const glm::vec3& rightUpForward);

    const glm::mat4& getViewMat() const;

private:

    void recalculateViewMat();
    glm::mat4 m_viewMat;

    glm::vec3 m_pos;
    glm::quat m_dirQuat{};
};

}
