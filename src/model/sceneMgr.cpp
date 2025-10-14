#include <model/sceneMgr.h>

#include <acre/utils/math/math.h>
#include <acre/render/renderer.h>

SceneMgr::SceneMgr(acre::Scene* scene) :
    m_scene(scene), m_tree(new acre::ResourceTree(scene))
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
    m_camera = m_tree->get<acre::CameraID>(0);
}

void SceneMgr::initDirectionLight()
{
    auto light       = new acre::SunLight;
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

    auto node    = m_tree->get<acre::LightID>(0);
    auto light   = node->ptr<acre::LightID>();
    light->light = point;
    light->type  = acre::LightType::Point;
}

void SceneMgr::clearScene()
{
    m_tree->clear();
    m_scene->clear();
    initCamera();
    initDirectionLight();
}

// void SceneMgr::clearHDR()
// {
//     for (auto index : m_extImageList)
//     {
//         auto node = m_tree->get<acre::ImageID>(index.idx);
//         m_tree->remove(node);
//     }
//     m_extImageList.resize(0);

//     for (auto index : m_extTextureList)
//     {
//         auto node = m_tree->get<acre::TextureID>(index.idx);
//         m_tree->remove(node);
//     }
//     m_extTextureList.resize(0);
// }

void SceneMgr::create(acre::component::DrawPtr draw)
{
    m_scene->createComponentDraw(draw);
}

void SceneMgr::remove(acre::Resource* node)
{
    if (!node || node->idx() == RESOURCE_ID_VALID) return;

    m_tree->remove(node);
}

void SceneMgr::aliveEntity(acre::EntityID id)
{
    auto node   = find<acre::EntityID>(id.idx);
    auto entity = (acre::Entity*)(id.ptr);
    entity->markAlive();
}

void SceneMgr::unAliveEntity(acre::EntityID id)
{
    auto node   = find<acre::EntityID>(id.idx);
    auto entity = node->ptr<acre::EntityID>();
    entity->unAlive();
}

void SceneMgr::update(acre::Resource* node, std::unordered_set<acre::Resource*>&& refs)
{
    if (!node || node->idx() == RESOURCE_ID_VALID) return;

    m_tree->update(node, std::move(refs));
}

void SceneMgr::incRefs(acre::Resource* node, std::unordered_set<acre::Resource*>&& refs)
{
    if (!node || node->idx() == RESOURCE_ID_VALID) return;

    m_tree->incRefs(node, std::move(refs));
}

void SceneMgr::update(acre::Resource* node)
{
    if (!node || node->idx() == RESOURCE_ID_VALID) return;

    m_tree->updateLeaf(node);
}

void SceneMgr::setMainCamera(uint32_t uuid)
{
    m_camera = find<acre::CameraID>(uuid);
}

void SceneMgr::highlightGeometry(uint32_t uuid)
{
    auto node = find<acre::GeometryID>(uuid);
    m_scene->highlight(node->id<acre::GeometryID>());
}

void SceneMgr::highlightMaterial(uint32_t uuid)
{
    auto node = find<acre::MaterialID>(uuid);
    m_scene->highlight(node->id<acre::MaterialID>());
}
