#pragma once

#include <acre/render/scene.h>
#include <model/camera.h>
#include <model/wrapper/resourceTree.h>
#include <model/animation.h>

#include <vector>

class SceneMgr
{
    acre::Scene*        m_scene;
    acre::ResourceTree* m_tree;
    acre::AnimationSet* m_animation_set = nullptr;

    acre::math::box3 m_box = acre::math::box3::empty();
    acre::Resource*  m_camera;

public:
    SceneMgr(acre::Scene*);

    ~SceneMgr();

    void clear_scene();
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

    auto get_box() { return m_box; }
    void reset_box() { m_box = acre::math::box3::empty(); }
    void merge_box(acre::math::box3 box) { m_box |= box; }

    const auto& camera_list() { return m_tree->_getMgr<acre::CameraID>(); }
    const auto& entity_list() { return m_tree->_getMgr<acre::EntityID>(); }
    const auto& light_list() { return m_tree->_getMgr<acre::LightID>(); }
    const auto& geometry_list() { return m_tree->_getMgr<acre::GeometryID>(); }
    const auto& material_list() { return m_tree->_getMgr<acre::MaterialID>(); }
    const auto& transform_list() { return m_tree->_getMgr<acre::TransformID>(); }

    auto camera_id() { return m_camera->id<acre::CameraID>(); }
    auto main_camera() { return m_camera; }
    void set_main_camera(uint32_t uuid);

    auto entity_count() { return m_tree->_getMgr<acre::EntityID>().size(); }
    void highlight_entity(acre::EntityID id) { m_scene->highlight(id); }
    void unhighlight_entity(acre::EntityID id) { m_scene->unhighlight(id); }
    void alive_entity(acre::EntityID id);
    void unalive_entity(acre::EntityID id);

    auto light_count() { return m_tree->_getMgr<acre::LightID>().size(); }
    auto get_light(acre::LightID id) { return m_tree->get<acre::LightID>(id.idx); }
    void update_light(acre::Resource* node) { m_tree->updateLeaf(node); }

    auto get_sun_light() { return m_scene->get_sun_light(); }
    void set_hdr_light(acre::HDRLight* light) { m_scene->set_hdr_light(light); }
    auto get_hdr_light() { return m_scene->get_hdr_light(); }
    void set_lut_ggx(acre::TextureID texture) { m_scene->set_lut_ggx(texture); }
    void set_lut_charlie(acre::TextureID texture) { m_scene->set_lut_charlie(texture); }
    void set_lut_sheen_albedo_scale(acre::TextureID texture) { m_scene->set_lut_sheen_albedo_scale(texture); }

    auto vindex_buffer(acre::VIndexID id) { return m_tree->get<acre::VIndexID>(id.idx); }
    auto vposition_buffer(acre::VPositionID id) { return m_tree->get<acre::VPositionID>(id.idx); }

    auto geometry_count() { return m_tree->_getMgr<acre::GeometryID>().size(); }
    auto get_geometry(acre::GeometryID id) { return m_tree->get<acre::GeometryID>(id.idx); }
    void highlight_geometry(acre::GeometryID id) { m_scene->highlight(id); }
    void unhighlight_geometry(acre::GeometryID id) { m_scene->unhighlight(id); }
    void highlight_geometry(uint32_t uuid);

    auto get_texture(acre::TextureID id) { return m_tree->get<acre::TextureID>(id.idx); }

    auto material_count() { return m_tree->_getMgr<acre::MaterialID>().size(); }
    auto get_material(acre::MaterialID id) { return m_tree->get<acre::MaterialID>(id.idx); }
    void highlight_material(acre::MaterialID id) { m_scene->highlight(id); }
    void unhighlight_material(acre::MaterialID id) { m_scene->unhighlight(id); }
    void highlight_material(uint32_t uuid);

    auto transform_count() { return m_tree->_getMgr<acre::TransformID>().size(); }
    auto get_transform(acre::TransformID id) { return m_tree->get<acre::TransformID>(id.idx); }

    auto texture_count() { return m_tree->_getMgr<acre::TextureID>().size(); }
    auto image_count() { return m_tree->_getMgr<acre::ImageID>().size(); }

    auto animation_set() const { return m_animation_set; }

private:
    void _init();

    void _init_camera();
    void _init_direction_light();
    void _init_point_light();
};
