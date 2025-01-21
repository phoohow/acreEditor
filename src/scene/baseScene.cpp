#include <scene/baseScene.h>

#include <acre/utils/math/math.h>

BaseScene::BaseScene(acre::Scene* scene) :
    m_scene(scene)
{
    createCamera();
    createDirectionLight();
    createPointLight();
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
    m_scene->setSunLight(light);
    m_scene->enableSunLight();
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
    auto camera   = std::make_shared<acre::Camera>();
    auto cameraID = m_scene->create(camera);
    m_cameras.emplace_back(cameraID);

    auto fov = 45.0f;
    camera->lookAt(acre::math::float3(0.0f, 0.0f, 1.0f), acre::math::float3(0.0f, 0.0f, 0.0f));
    camera->perspective(fov, 1.0f, 1e-1f, 1e6f);
    m_scene->setMainCamera(cameraID);

    // auto entity   = std::make_shared<acre::Entity>("camera");
    // auto entityID = m_scene->create(entity);
    // m_entities.emplace_back(entityID);
    // m_scene->create(entityID, cameraID);
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
