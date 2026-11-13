/**
 *  @author    Dubsky Tomas
 */
module realengine:graphics.cameras.FlyingCamera3D_impl;
import glm;
import :graphics.cameras.FlyingCamera3D;

namespace re {

FlyingCamera3D::FlyingCamera3D(glm::vec3 pos, glm::vec3 yawPitchRoll)
    : FlyingCamera3D(pos, glm::quat{yawPitchRoll}) {
}

FlyingCamera3D::FlyingCamera3D(glm::vec3 pos, glm::quat dirquat)
    : m_viewMat(calculateViewMat(dirquat, pos))
    , m_pos(pos)
    , m_dirQuat(dirquat) {
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
    return glm::gtc::translate(glm::gtc::mat4_cast(dirQuat), -pos);
}

} // namespace re
