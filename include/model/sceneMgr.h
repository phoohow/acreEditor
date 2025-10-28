#pragma once

#include <acre/render/scene.h>
#include <model/camera.h>
#include <model/wrapper/resourceTree.h>
#include <model/animation.h>

#include <controller/animationController.h>

#include <vector>

class SceneMgr
{
    acre::Scene*        m_scene;
    acre::ResourceTree* m_tree;
    acre::AnimationSet* m_animationSet = nullptr;

    acre::math::box3     m_box = acre::math::box3::empty();
    acre::Resource*      m_camera;
    AnimationController* m_animController = nullptr;

public:
    SceneMgr(acre::Scene*);

    ~SceneMgr();

    void clearScene();
    // void clearHDR();

    template <typename ID>
    auto create(acre::UUID uuid)
    {
        return m_tree->get<ID>(uuid);
    }

    template <typename ID>
    acre::Resource* find(acre::UUID uuid)
    {
        if (m_tree->has<ID>(uuid))
            return m_tree->get<ID>(uuid);

        return nullptr;
    }

    void update(acre::Resource* node, std::unordered_set<acre::Resource*>&& refs);
    void incRefs(acre::Resource* node, std::unordered_set<acre::Resource*>&& refs);
    void update(acre::Resource* node);
    template <typename ID>
    void update(acre::UUID uuid)
    {
        auto node = find<ID>(uuid);
        update(node);
    }

    void remove(acre::Resource* node);
    template <typename ID>
    void remove(acre::UUID uuid)
    {
        auto node = find<ID>(uuid);
        remove(node);
    }

    void create(acre::component::DrawPtr);

    auto getBox() { return m_box; }
    void resetBox() { m_box = acre::math::box3::empty(); }
    void mergeBox(acre::math::box3 box) { m_box |= box; }

    const auto& getCameraList() { return m_tree->_getMgr<acre::CameraID>(); }
    const auto& getEntityList() { return m_tree->_getMgr<acre::EntityID>(); }
    const auto& getLightList() { return m_tree->_getMgr<acre::LightID>(); }
    const auto& getGeometryList() { return m_tree->_getMgr<acre::GeometryID>(); }
    const auto& getMaterialList() { return m_tree->_getMgr<acre::MaterialID>(); }
    const auto& getTransformList() { return m_tree->_getMgr<acre::TransformID>(); }

    auto getCameraID() { return m_camera->id<acre::CameraID>(); }
    auto getMainCamera() { return m_camera; }
    void setMainCamera(uint32_t uuid);

    auto getEntityCount() { return m_tree->_getMgr<acre::EntityID>().size(); }
    void highlightEntity(acre::EntityID id) { m_scene->highlight(id); }
    void unhighlightEntity(acre::EntityID id) { m_scene->unhighlight(id); }
    void aliveEntity(acre::EntityID id);
    void unAliveEntity(acre::EntityID id);

    auto getLightCount() { return m_tree->_getMgr<acre::LightID>().size(); }
    auto getLight(acre::LightID id) { return m_tree->get<acre::LightID>(id.idx); }
    void updateLight(acre::Resource* node) { m_tree->updateLeaf(node); }

    auto get_sun_light() { return m_scene->get_sun_light(); }
    void set_hdr_light(acre::HDRLight* light) { m_scene->set_hdr_light(light); }
    auto get_hdr_light() { return m_scene->get_hdr_light(); }
    void set_lut_ggx(acre::TextureID texture) { m_scene->set_lut_ggx(texture); }
    void set_lut_charlie(acre::TextureID texture) { m_scene->set_lut_charlie(texture); }
    void set_lut_sheen_albedo_scale(acre::TextureID texture) { m_scene->set_lut_sheen_albedo_scale(texture); }

    auto getVIndexBuffer(acre::VIndexID id) { return m_tree->get<acre::VIndexID>(id.idx); }
    auto getVPositionBuffer(acre::VPositionID id) { return m_tree->get<acre::VPositionID>(id.idx); }

    auto getGeometryCount() { return m_tree->_getMgr<acre::GeometryID>().size(); }
    auto getGeometry(acre::GeometryID id) { return m_tree->get<acre::GeometryID>(id.idx); }
    void highlightGeometry(acre::GeometryID id) { m_scene->highlight(id); }
    void unhighlightGeometry(acre::GeometryID id) { m_scene->unhighlight(id); }
    void highlightGeometry(uint32_t uuid);

    auto getTexture(acre::TextureID id) { return m_tree->get<acre::TextureID>(id.idx); }

    auto getMaterialCount() { return m_tree->_getMgr<acre::MaterialID>().size(); }
    auto getMaterial(acre::MaterialID id) { return m_tree->get<acre::MaterialID>(id.idx); }
    void highlightMaterial(acre::MaterialID id) { m_scene->highlight(id); }
    void unhighlightMaterial(acre::MaterialID id) { m_scene->unhighlight(id); }
    void highlightMaterial(uint32_t uuid);

    auto getTransformCount() { return m_tree->_getMgr<acre::TransformID>().size(); }
    auto getTransform(acre::TransformID id) { return m_tree->get<acre::TransformID>(id.idx); }

    auto getTextureCount() { return m_tree->_getMgr<acre::TextureID>().size(); }
    auto getImageCount() { return m_tree->_getMgr<acre::ImageID>().size(); }

    void updateAnimation(float deltaTime);
    void setAnimationController(AnimationController* controller);
    auto getAnimationController() const { return m_animController; }
    auto getAnimationSet() const { return m_animationSet; }

private:
    void _init();

    void initCamera();
    void initDirectionLight();
    void initPointLight();
};
