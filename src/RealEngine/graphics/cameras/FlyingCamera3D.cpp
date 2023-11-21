/*!
 *  @author    Dubsky Tomas
 */
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <RealEngine/graphics/cameras/FlyingCamera3D.hpp>

namespace re {

FlyingCamera3D::FlyingCamera3D(glm::vec3 pos, glm::vec3 yawPitchRoll)
    : m_pos(pos)
    , m_dirQuat(yawPitchRoll)
    , m_viewMat(calculateViewMat(m_dirQuat, m_pos)) {
}

void FlyingCamera3D::rotate(glm::vec3 yawPitchRoll) {
    m_dirQuat = glm::normalize(glm::quat{yawPitchRoll} * m_dirQuat);
    m_viewMat = calculateViewMat(m_dirQuat, m_pos);
}

void FlyingCamera3D::move(glm::vec3 rightUpBack) {
    m_pos += rightUpBack * m_dirQuat;
    m_viewMat = calculateViewMat(m_dirQuat, m_pos);
}

glm::mat4 FlyingCamera3D::calculateRelativeViewMat(
    glm::vec3 yawPitchRoll, glm::vec3 rightUpBack
) const {
    glm::quat dirQuat = glm::normalize(glm::quat{yawPitchRoll} * m_dirQuat);
    return calculateViewMat(dirQuat, m_pos + (rightUpBack * dirQuat));
}

glm::mat4 FlyingCamera3D::calculateViewMat(glm::quat dirQuat, glm::vec3 pos) const {
    return glm::translate(glm::toMat4(dirQuat), -pos);
}

} // namespace re
