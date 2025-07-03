#include <controller/cameraController.h>

static constexpr double M_PI          = 3.14159265358979;
static constexpr float  g_fov         = 60;
static constexpr float  g_far         = 1e6f;
static constexpr float  g_rotateSpeed = 1.0f;

static void setView(Camera* camera, acre::math::box3 box, acre::math::float3 dir)
{
    auto fov      = g_fov;
    auto radius   = acre::math::length(box.diagonal()) * 0.5f;
    auto distance = radius / sinf(acre::math::radians(fov * 0.5f));
    camera->setPosition(box.center() + dir * distance);
    camera->setTarget(box.center());
    camera->resetYaw();
    camera->resetPitch();
}

CameraController::CameraController(SceneMgr* sceneMgr) :
    m_scene(sceneMgr)
{
    init();
}

CameraController::~CameraController()
{
}

void CameraController::init()
{
    m_camera = new Camera;
    m_camera->setPosition(acre::math::float3(0.0f, 0.0f, 1.0f));
    m_camera->setTarget(0.0f);
    m_camera->setUp(acre::math::float3(0.0f, 1.0f, 0.0f));

    m_camera->setFOV(45.0f);
    m_camera->setAspect(1.0f);
    m_camera->setNear(1e-1f);
    m_camera->setFar(1e6f);

    syncCamera();
}

void CameraController::reset()
{
    auto fov    = g_fov;
    auto radius = acre::math::length(m_scene->getBox().diagonal()) * 0.5f;

    auto mainCamera = m_scene->getMainCamera();
    if (mainCamera->type == acre::Camera::ProjectType::tPerspective)
    {
        m_camera->setFOV(fov);
        m_camera->setAspect(m_width / m_height);
        m_camera->setNear(radius * 0.1);
        m_camera->setFar(g_far);
    }
    else
    {
        m_camera->setNear(radius * 0.1);
        m_camera->setFar(g_far);
        m_camera->setLeft(-radius);
        m_camera->setRight(radius);
        m_camera->setTop(radius);
        m_camera->setBottom(-radius);
    }

    frontView();
}

void CameraController::resize(float width, float height)
{
    m_width  = width;
    m_height = height;

    reset();
}

void CameraController::move(float x, float y, float z)
{
    auto radius = acre::math::length(m_scene->getBox().diagonal()) * 0.5f;
    m_camera->translate(acre::math::float3(x, y, z) * radius * 0.02f);

    syncCamera();
}

void CameraController::moveForward()
{
    auto dir = acre::math::normalize(m_camera->getTarget() - m_camera->getPosition());
    move(dir.x, dir.y, dir.z);
}

void CameraController::moveBack()
{
    auto dir = acre::math::normalize(m_camera->getTarget() - m_camera->getPosition());
    move(-dir.x, -dir.y, -dir.z);
}

void CameraController::rotateY(float degree)
{
    m_camera->rotate(0, acre::math::radians(degree * g_rotateSpeed));

    syncCamera();
}

void CameraController::rotateX(float degree)
{
    m_camera->rotate(acre::math::radians(degree * g_rotateSpeed), 0);

    syncCamera();
}

void CameraController::leftView()
{
    setView(m_camera, m_scene->getBox(), acre::math::float3(1, 0, 0));
    m_camera->rotate(0, M_PI * 1.5);

    syncCamera();
}

void CameraController::rightView()
{
    setView(m_camera, m_scene->getBox(), acre::math::float3(-1, 0, 0));
    m_camera->rotate(0, M_PI * 0.5);

    syncCamera();
}

void CameraController::frontView()
{
    setView(m_camera, m_scene->getBox(), acre::math::float3(0, 0, 1));
    m_camera->rotate(0, M_PI);

    syncCamera();
}

void CameraController::backView()
{
    setView(m_camera, m_scene->getBox(), acre::math::float3(0, 0, -1));
    m_camera->rotate(0, 0);

    syncCamera();
}

void CameraController::topView()
{
    setView(m_camera, m_scene->getBox(), acre::math::float3(0, 1, 0));
    m_camera->rotate(M_PI * 0.5, 0);

    syncCamera();
}

void CameraController::bottomView()
{
    setView(m_camera, m_scene->getBox(), acre::math::float3(0, -1, 0));
    m_camera->rotate(-M_PI * 0.5, 0);

    syncCamera();
}

void CameraController::syncCamera()
{
    auto mainCamera      = m_scene->getMainCamera();
    mainCamera->position = m_camera->getPosition();
    mainCamera->target   = m_camera->getTarget();
    mainCamera->up       = m_camera->getUp();

    if (mainCamera->type == acre::Camera::ProjectType::tPerspective)
    {
        auto& projection     = std::get<acre::Camera::Perspective>(mainCamera->projection);
        projection.fov       = m_camera->getFOV();
        projection.aspect    = m_camera->getAspect();
        projection.nearPlane = m_camera->getNear();
        projection.farPlane  = m_camera->getFar();
    }
    else
    {
        auto& projection       = std::get<acre::Camera::Orthonormal>(mainCamera->projection);
        projection.nearPlane   = m_camera->getNear();
        projection.farPlane    = m_camera->getFar();
        projection.topPlane    = m_camera->getTop();
        projection.bottomPlane = m_camera->getBottom();
        projection.leftPlane   = m_camera->getLeft();
        projection.rightPlane  = m_camera->getRight();
    }
}
