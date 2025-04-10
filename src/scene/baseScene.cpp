#include <scene/baseScene.h>

#include <acre/utils/math/math.h>

BaseScene::BaseScene(acre::Scene* scene) :
    m_scene(scene)
{
    createCamera();
    createDirectionLight();
    // createPointLight();
}

BaseScene::~BaseScene()
{
}

void BaseScene::createDirectionLight()
{
    auto light       = std::make_shared<acre::SunLight>();
    light->color     = acre::math::float3(1.0, 1.0, 1.0);
    light->direction = acre::math::normalize(acre::math::float3(0, -1, -1));
    light->factor    = 1.0;
    light->enable    = true;
    m_scene->setSunLight(light);
}

void BaseScene::createPointLight()
{
    acre::PointLight point;
    point.position = acre::math::float3(0, 1, 1);

    auto light   = std::make_shared<acre::Light>();
    light->light = point;
    light->type  = acre::LightType::Point;
    m_lights.emplace_back(m_scene->create(light));
}

void BaseScene::createCamera()
{
    m_camera = new Camera;

    auto mainCamera = std::make_shared<acre::Camera>();
    m_cameraID      = m_scene->create(mainCamera);
    m_cameras.emplace_back(m_cameraID);

    m_camera->setPosition(acre::math::float3(0.0f, 0.0f, 1.0f));
    m_camera->setTarget(0.0f);
    m_camera->setUp(acre::math::float3(0.0f, 1.0f, 0.0f));
    m_camera->setFOV(45.0f);
    m_camera->setAspect(1.0f);
    m_camera->setNear(1e-1f);
    m_camera->setFar(1e6f);

    swapCamera();
}

void BaseScene::swapCamera()
{
    auto mainCamera      = getMainCamera();
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
        auto& projection     = std::get<acre::Camera::Orthonormal>(mainCamera->projection);
        projection.nearPlane = m_camera->getNear();
        projection.farPlane  = m_camera->getFar();
        // projection.topPlane    = m_camera->getTop();
        // projection.bottomPlane = m_camera->getBottom();
        // projection.leftPlane   = m_camera->getLeft();
        // projection.rightPlane  = m_camera->getRight();
    }
}

void BaseScene::clearScene()
{
    for (auto id : m_entities)
        m_scene->removeEntity(id);
    m_entities.resize(0);

    for (auto id : m_geometrys)
        m_scene->removeGeometry(id);
    m_geometrys.resize(0);

    for (auto id : m_materials)
        m_scene->removeMaterial(id);
    m_materials.resize(0);

    for (auto id : m_transforms)
        m_scene->removeTransform(id);
    m_transforms.resize(0);

    for (auto id : m_textures)
        m_scene->removeTexture(id);
    m_textures.resize(0);

    for (auto id : m_images)
        m_scene->removeImage(id);
    m_images.resize(0);

    for (auto id : m_samplers)
        m_scene->removeSampler(id);
    m_samplers.resize(0);
}

void BaseScene::clearHDR()
{
    for (auto index : m_imageExts)
        m_scene->removeImage(index);
    m_imageExts.resize(0);

    for (auto index : m_textureExts)
        m_scene->removeTexture(index);
    m_textureExts.resize(0);
}
