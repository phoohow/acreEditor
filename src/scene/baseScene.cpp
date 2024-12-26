#include <scene/baseScene.h>
#include <Render/utils/math/math.h>

BaseScene::BaseScene(rdr::Scene* scene) :
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
    auto light       = std::make_shared<rdr::Light>();
    light->color     = rdr::math::float3(1.0, 1.0, 1.0);
    light->direction = rdr::math::normalize(rdr::math::float3(0, -1, -1));
    light->factor    = 1.0;
    light->type      = rdr::LightType::Direction;
    m_lights.emplace_back(m_scene->create(light));
}

void BaseScene::createPointLight()
{
    auto light      = std::make_shared<rdr::Light>();
    light->position = rdr::math::float3(0, 1, 1);
    light->type     = rdr::LightType::Point;
    m_lights.emplace_back(m_scene->create(light));
}

void BaseScene::createCamera()
{
    auto camera   = std::make_shared<rdr::Camera>();
    auto cameraID = m_scene->create(camera);
    m_cameras.emplace_back(cameraID);

    auto fov = 45.0f;
    camera->lookAt(rdr::math::float3(0.0f, 0.0f, 1.0f), rdr::math::float3(0.0f, 0.0f, 0.0f));
    camera->perspective(fov, 1.0f, 1e-1f, 1e6f);
    m_scene->setMainCamera(cameraID);

    // auto entity   = std::make_shared<rdr::Entity>("camera");
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

    for (auto index : m_lightExts)
        m_scene->removeLight(index);
    m_lightExts.resize(0);
}
