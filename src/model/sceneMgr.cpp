#include <model/sceneMgr.h>

#include <acre/utils/math/math.h>
#include <acre/render/renderer.h>

SceneMgr::SceneMgr(acre::Scene* scene) :
    m_scene(scene)
{
    initCamera();
    initDirectionLight();
    // initPointLight();
}

SceneMgr::~SceneMgr()
{
}

void SceneMgr::initCamera()
{
    auto mainCamera = std::make_shared<acre::Camera>();
    m_cameraID      = m_scene->create(mainCamera);
    m_cameras.emplace_back(m_cameraID);
}

void SceneMgr::initDirectionLight()
{
    auto light       = acre::createSunLight();
    light->color     = acre::math::float3(1.0, 1.0, 1.0);
    light->direction = acre::math::normalize(acre::math::float3(0, -1, -1));
    light->factor    = 1.0;
    light->enable    = true;
    m_scene->setSunLight(light);
}

void SceneMgr::initPointLight()
{
    acre::PointLight point;
    point.position = acre::math::float3(0, 1, 1);

    auto light   = acre::createLight();
    light->light = point;
    light->type  = acre::LightType::Point;
    m_lights.emplace_back(m_scene->create(light));
}

void SceneMgr::clearScene()
{
    m_entities.resize(0);
    m_geometries.resize(0);
    m_materials.resize(0);
    m_transforms.resize(0);
    m_textures.resize(0);
    m_images.resize(0);
    m_samplers.resize(0);

    m_scene->clear();

    initCamera();
    initDirectionLight();
}

void SceneMgr::clearHDR()
{
    for (auto index : m_extImageList)
        m_scene->removeImage(index);
    m_extImageList.resize(0);

    for (auto index : m_extTextureList)
        m_scene->removeTexture(index);
    m_extTextureList.resize(0);
}

acre::UintBufferID SceneMgr::createVIndex(acre::UintBufferPtr buffer)
{
    return m_scene->createVIndex(buffer);
}

acre::Float3BufferID SceneMgr::createVPosition(acre::Float3BufferPtr buffer)
{
    return m_scene->createVPosition(buffer);
}

acre::Float2BufferID SceneMgr::createVUV(acre::Float2BufferPtr buffer)
{
    return m_scene->createVUV(buffer);
}

acre::Float3BufferID SceneMgr::createVNormal(acre::Float3BufferPtr buffer)
{
    return m_scene->createVNormal(buffer);
}

acre::Float4BufferID SceneMgr::createVTangent(acre::Float4BufferPtr buffer)
{
    return m_scene->createVTangent(buffer);
}

acre::Float4BufferID SceneMgr::createVColor(acre::Float4BufferPtr buffer)
{
    return m_scene->createVColor(buffer);
}

acre::ImageID SceneMgr::create(acre::ImagePtr image)
{
    auto id = m_scene->create(image);
    m_images.push_back(id);
    return id;
}

acre::TextureID SceneMgr::create(acre::TexturePtr texture)
{
    auto id = m_scene->create(texture);
    m_textures.push_back(id);
    return id;
}

acre::GeometryID SceneMgr::create(acre::GeometryPtr geometry)
{
    auto id = m_scene->create(geometry);
    m_geometries.push_back(id);
    return id;
}

acre::MaterialID SceneMgr::create(acre::MaterialPtr material)
{
    auto id = m_scene->create(material);
    m_materials.push_back(id);
    return id;
}

acre::TransformID SceneMgr::create(acre::TransformPtr transform)
{
    auto id = m_scene->create(transform);
    m_transforms.push_back(id);
    return id;
}

acre::EntityID SceneMgr::create(acre::EntityPtr entity)
{
    auto id = m_scene->create(entity);
    m_entities.push_back(id);
    return id;
}

void SceneMgr::create(acre::component::DrawPtr draw)
{
    m_scene->createComponentDraw(draw);
}

acre::ImageID SceneMgr::createExt(acre::ImagePtr image)
{
    auto id = m_scene->createImmortal(image);
    m_extImageList.push_back(id);
    return id;
}

acre::TextureID SceneMgr::createExt(acre::TexturePtr texture)
{
    auto id = m_scene->createImmortal(texture);
    m_extTextureList.push_back(id);
    return id;
}

acre::TransformID SceneMgr::createExt(acre::TransformPtr transform)
{
    auto id = m_scene->create(transform);
    m_extTransforms.push_back(id);
    return id;
}

acre::ImageID SceneMgr::findImage(uint32_t index)
{
    return index < m_images.size() ? m_images[index] : RESOURCE_ID_VALID;
}

acre::TextureID SceneMgr::findTexture(uint32_t index)
{
    return index < m_textures.size() ? m_textures[index] : RESOURCE_ID_VALID;
}

acre::GeometryID SceneMgr::findGeometry(uint32_t index)
{
    return index < m_geometries.size() ? m_geometries[index] : RESOURCE_ID_VALID;
}

acre::MaterialID SceneMgr::findMaterial(uint32_t index)
{
    return index < m_materials.size() ? m_materials[index] : RESOURCE_ID_VALID;
}

acre::TransformID SceneMgr::findTransform(uint32_t index)
{
    return index < m_transforms.size() ? m_transforms[index] : RESOURCE_ID_VALID;
}

acre::EntityID SceneMgr::findEntity(uint32_t index)
{
    return index < m_entities.size() ? m_entities[index] : RESOURCE_ID_VALID;
}
