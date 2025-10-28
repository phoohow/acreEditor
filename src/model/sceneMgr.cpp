#include <model/sceneMgr.h>

#include <acre/utils/math/math.h>
#include <acre/render/renderer.h>

SceneMgr::SceneMgr(acre::Scene* scene) :
    m_scene(scene), m_tree(new acre::ResourceTree(scene))
{
    initCamera();
    initDirectionLight();
    // initPointLight();

    m_animationSet   = new acre::AnimationSet();
    m_animController = new AnimationController(m_animationSet);
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
    m_scene->set_sun_light(light);
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
    m_scene->create_component_draw(draw);
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
    entity->mark_alive();
}

void SceneMgr::unAliveEntity(acre::EntityID id)
{
    auto node   = find<acre::EntityID>(id.idx);
    auto entity = node->ptr<acre::EntityID>();
    entity->reset_alive();
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

void SceneMgr::updateAnimation(float deltaTime)
{
    if (m_animController)
    {
        // Sample animation and apply to Transform nodes
        // Assume AnimationController::update already samples each channel value
        // Iterate current animation channels, find corresponding Transform node and set property
        m_animController->update(deltaTime);

        const auto animation = m_animController->getCurrentAnimation();
        if (animation)
        {
            const auto& channels = animation->channels;
            for (size_t i = 0; i < channels.size(); ++i)
            {
                const auto& channel = channels[i];
                auto        node    = m_tree->get<acre::TransformID>(channel.targetNode);
                if (!node) continue;
                auto transform = node->ptr<acre::TransformID>();
                if (!transform) continue;

                const auto& value  = m_animController->getSampledValue(i);
                auto        affine = acre::math::affine3::identity();
                if (channel.targetPath == "translation" && value.size() >= 3)
                {
                    transform->translation = acre::math::float3(value[0], value[1], value[2]);
                    affine *= acre::math::translation(transform->translation);
                }
                else if (channel.targetPath == "rotation" && value.size() >= 4)
                {
                    transform->ratation = acre::math::float4(value[0], value[1], value[2], value[3]);
                    auto rotate         = acre::math::quat(value[3], value[0], value[1], value[2]);
                    affine *= rotate.toAffine();
                }
                else if (channel.targetPath == "scale" && value.size() >= 3)
                {
                    transform->scale = acre::math::float3(value[0], value[1], value[2]);
                    affine *= acre::math::scaling(transform->scale);
                }

                // Update Transform matrix
                transform->affine = affine;
                transform->matrix = acre::math::affineToHomogeneous(affine);
                update(node);

                auto skin = find<acre::SkinID>(channel.targetNode);
                if (skin)
                {
                    auto inverse_node_matrix                = skin->ptr<acre::SkinID>()->inverse_node_matrix;
                    auto inverse_bind_matrix                = skin->ptr<acre::SkinID>()->inverse_bind_matrix;
                    skin->ptr<acre::SkinID>()->joint_matrix = inverse_bind_matrix * transform->matrix * inverse_node_matrix;
                    skin->ptr<acre::SkinID>()->joint_affine = acre::math::homogeneousToAffine(skin->ptr<acre::SkinID>()->joint_matrix);
                    update(skin);
                }
            }
        }
    }
}
