#include <acre/utils/math/math.h>

class Camera
{
    acre::math::float3 m_position = {0.0f, 0.0f, 0.0f};
    acre::math::float3 m_target   = {0.0f, 0.0f, 0.0f};
    acre::math::float3 m_upDir    = {0.0f, 1.0f, 0.0f};
    acre::math::float3 m_front    = {0.0f, 0.0f, -1.0f};
    // acre::math::float3 m_rightDir = {0.0f, 0.0f, 0.0f};

    float m_fov         = 90.0f;
    float m_aspect      = 1.0f;
    float m_nearPlane   = 0.0f;
    float m_farPlane    = 0.0f;
    float m_leftPlane   = 0.0f;
    float m_rightPlane  = 0.0f;
    float m_topPlane    = 0.0f;
    float m_bottomPlane = 0.0f;

    // Interaction
    float m_yaw      = 0.f;
    float m_pitch    = 0.f;
    float m_roll     = 0.f;
    float m_distance = 0.f;

public:
    enum class ProjectType
    {
        Perspective,
        Orthonormal,
    };

    // Near and far plane value
    enum class ClipMode
    {
        Regulated,
        Adaptive,
    };

    void setProjectType(ProjectType type) { m_type = type; }
    void setClipMode(ClipMode mode) { m_clipMode = mode; }

    // pitch: around x axis,  yaw: around y axis,  roll: round z axis
    void rotate(float pitch, float yaw, float roll = 0);
    void translate(const acre::math::float3& value);

    const auto& getProjectType() const { return m_type; }
    const auto& getPosition() const { return m_position; }
    const auto& getTarget() const { return m_target; }
    const auto& getUp() const { return m_upDir; }
    const auto& getFront() const { return m_front; }
    const auto& getFOV() const { return m_fov; }
    const auto& getAspect() const { return m_aspect; }
    const auto& getNear() const { return m_nearPlane; }
    const auto& getFar() const { return m_farPlane; }
    const auto& getLeft() const { return m_leftPlane; }
    const auto& getRight() const { return m_rightPlane; }
    const auto& getTop() const { return m_topPlane; }
    const auto& getBottom() const { return m_bottomPlane; }

    void setPosition(const acre::math::float3& value);
    void setTarget(const acre::math::float3& value);
    void setUp(const acre::math::float3& value);

    void setFOV(float value);
    void setAspect(float value);
    void setNear(float value);
    void setFar(float value);
    void setLeft(float value);
    void setRight(float value);
    void setTop(float value);
    void setBottom(float value);

    void setYaw(float value) { m_yaw = value; }
    void setPitch(float value) { m_pitch = value; }
    void resetYaw() { m_yaw = 0; }
    void resetPitch() { m_pitch = 0; }

private:
    ProjectType m_type     = ProjectType::Perspective;
    ClipMode    m_clipMode = ClipMode::Regulated;
};
