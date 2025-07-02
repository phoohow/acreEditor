#include <model/camera.h>

using namespace acre;

void Camera::rotate(float pitch, float yaw, float roll)
{
    m_pitch += pitch;
    m_yaw -= yaw;
    m_roll += roll;

    auto rotateMatrix = math::rotationQuat(math::float3(m_pitch, m_yaw, m_roll)).toMatrix();

    m_position = -rotateMatrix.row2 * m_distance + m_target;
    // m_rightDir  = -rotateMatrix.row0;
    m_upDir = rotateMatrix.row1;
    // m_front = rotateMatrix.row2;
}

void Camera::translate(const math::float3& offset)
{
    m_position += offset;
    m_distance = math::length(m_position - m_target);
    m_target += offset;
}

void Camera::setPosition(const math::float3& value)
{
    m_position = value;
    m_distance = math::length(m_position - m_target);
}

void Camera::setTarget(const math::float3& value)
{
    m_target   = value;
    m_distance = math::length(m_position - m_target);
}

void Camera::setUp(const math::float3& value)
{
    m_upDir = value;
}

void Camera::setFOV(float value)
{
    m_fov = value;
}

void Camera::setAspect(float value)
{
    m_aspect = value;
}

void Camera::setNear(float value)
{
    m_nearPlane = value;
}

void Camera::setFar(float value)
{
    m_farPlane = value;
}

void Camera::setTop(float value)
{
    m_topPlane = value;
}

void Camera::setBottom(float value)
{
    m_bottomPlane = value;
}

void Camera::setLeft(float value)
{
    m_leftPlane = value;
}

void Camera::setRight(float value)
{
    m_rightPlane = value;
}
