/*!
 *  @author    Dubsky Tomas
 */
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <RealEngine/graphics/cameras/FlyingCamera3D.hpp>

namespace re {

FlyingCamera3D::FlyingCamera3D(const glm::vec3& pos, const glm::vec3& yawPitchRoll)
    : m_pos(pos)
    , m_dirQuat(yawPitchRoll) {
    recalculateViewMat();
}

void FlyingCamera3D::rotate(const glm::vec3& yawPitchRoll) {
    m_dirQuat = glm::normalize(glm::quat{yawPitchRoll} * m_dirQuat);
    recalculateViewMat();
}

void FlyingCamera3D::move(const glm::vec3& rightUpBack) {
    m_pos += rightUpBack * m_dirQuat;
    recalculateViewMat();
}

const glm::mat4& FlyingCamera3D::viewMat() const {
    return m_viewMat;
}

void FlyingCamera3D::recalculateViewMat() {
    m_viewMat = glm::translate(glm::toMat4(m_dirQuat), -m_pos);
}

} // namespace re
