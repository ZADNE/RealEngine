/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/cameras/FlyingCamera3D.hpp>

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>

namespace re {

FlyingCamera3D::FlyingCamera3D(const glm::vec3& pos, const glm::vec3& yawPitchRoll):
    m_pos(pos), m_dirQuat(yawPitchRoll) {
    recalculateViewMat();
}

void FlyingCamera3D::rotate(const glm::vec3& yawPitchRoll) {
    m_dirQuat = glm::quat(yawPitchRoll) * m_dirQuat;
    m_dirQuat = glm::normalize(m_dirQuat);
    recalculateViewMat();
}

void FlyingCamera3D::move(const glm::vec3& rightUpForward) {
    m_pos += (glm::vec3(-rightUpForward.x, rightUpForward.y, rightUpForward.z) * m_dirQuat);
    recalculateViewMat();
}

const glm::mat4& FlyingCamera3D::viewMat() const {
    return m_viewMat;
}

/*void FlyingCamera3D::recalculateDirections() {
    float sy = sin(m_yawPitchRoll.x);//sin(yaw)
    float cy = cos(m_yawPitchRoll.x);//cos(yaw)
    float sp = sin(m_yawPitchRoll.y);//sin(pitch)
    float cp = cos(m_yawPitchRoll.y);//cos(pitch)

    m_dir.x = cy * cp;
    m_dir.y = sp;
    m_dir.z = sy * cp;
    m_dir = glm::normalize(m_dir);
    m_right = glm::normalize(glm::cross(m_dir, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_dir));
}*/

void FlyingCamera3D::recalculateViewMat() {
    m_viewMat = glm::toMat4(m_dirQuat);
    m_viewMat = glm::translate(m_viewMat, m_pos);
}

}
