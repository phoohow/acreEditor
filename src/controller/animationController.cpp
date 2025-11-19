#include <controller/animationController.h>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

AnimationController::AnimationController(SceneMgr* scene) :
    m_scene(scene), m_current(nullptr), m_time(0.0f)
{}

void AnimationController::play(const std::string& name)
{
    m_current = m_scene->animation_set()->animation(name);
    m_time    = 0.0f;
}

void AnimationController::stop()
{
    if (!m_current) return;

    if (m_time >= 0.0f)
    {
        m_time = 0.0f;
        update(0.0f);
    }
}

void AnimationController::update(float delta_time)
{
    if (!m_current) _try_play();
    if (!m_current) return;

    m_time += delta_time;
    if (m_time > m_current->duration)
    {
        // m_time = m_current->duration;
        m_time = 0.0f;
    }

    _update_sampled_values();

    _update_scene();
}

const std::vector<float>& AnimationController::sampled_value(size_t channel_idx) const
{
    static const std::vector<float> empty;
    if (channel_idx < m_sampled_values.size())
        return m_sampled_values[channel_idx];
    return empty;
}

void AnimationController::_try_play()
{
    if (!m_current && m_scene->animation_set() && !m_scene->animation_set()->animations.empty())
    {
        m_current = m_scene->animation_set()->animation(0);
        m_time    = 0.0f;
    }
}

void AnimationController::_update_sampled_values()
{
    // Sample and cache each channel's animation value (linear interpolation only)
    m_sampled_values.clear();
    for (const auto& channel : m_current->channels)
    {
        const auto& sampler    = m_current->samplers[channel.sampler_idx];
        size_t      frameCount = sampler.input.size();
        if (frameCount < 2)
        {
            m_sampled_values.push_back({});
            continue;
        }

        size_t idx = 0;
        while (idx + 1 < frameCount && m_time > sampler.input[idx + 1])
            ++idx;
        if (idx >= frameCount - 1) idx = frameCount - 2;

        float       t0     = sampler.input[idx];
        float       t1     = sampler.input[idx + 1];
        const auto& v0     = sampler.output[idx];
        const auto& v1     = sampler.output[idx + 1];
        float       localT = (t1 > t0) ? (m_time - t0) / (t1 - t0) : 0.0f;

        std::vector<float> value(v0.size());
        for (size_t i = 0; i < v0.size(); ++i)
            value[i] = v0[i] + (v1[i] - v0[i]) * localT;

        m_sampled_values.push_back(value);
    }
}

void AnimationController::_update_scene()
{
    const auto& channels = m_current->channels;

    // First pass: apply sampled components into each node's local TRS fields
    for (size_t idx = 0; idx < channels.size(); ++idx)
    {
        const auto& channel = channels[idx];
        const auto& value   = sampled_value(idx);
        if (value.empty()) continue;
        auto node = m_scene->find<acre::TransformID>(channel.target_node);
        if (!node) continue;
        auto trs = node->ptr<acre::TransformID>();
        if (!trs) continue;

        if (channel.target_path == "scale" && value.size() >= 3)
        {
            trs->scale = acre::math::float3(value[0], value[1], value[2]);
        }
        else if (channel.target_path == "rotation" && value.size() >= 4)
        {
            trs->rotation = acre::math::quat(value[3], value[0], value[1], value[2]);
        }
        else if (channel.target_path == "translation" && value.size() >= 3)
        {
            trs->translation = acre::math::float3(value[0], value[1], value[2]);
        }
    }

    // Second pass: collect affected nodes, deduplicate and process from root-to-leaf
    std::unordered_set<int> affected_nodes;
    for (const auto& ch : channels)
        affected_nodes.insert(ch.target_node);

    std::vector<int> nodes(affected_nodes.begin(), affected_nodes.end());
    // cache depths to avoid repeated scene lookups
    std::unordered_map<int, int> depth_cache;
    auto                         compute_depth = [&](int nid) -> int {
        auto it = depth_cache.find(nid);
        if (it != depth_cache.end()) return it->second;
        int  d = 0;
        auto n = m_scene->find<acre::TransformID>(nid);
        while (n && n->parent)
        {
            ++d;
            n = m_scene->find<acre::TransformID>(n->parent->uuid());
        }
        depth_cache[nid] = d;
        return d;
    };

    std::sort(nodes.begin(), nodes.end(), [&](int a, int b) {
        return compute_depth(a) < compute_depth(b);
    });

    for (int node_idx : nodes)
    {
        auto node = m_scene->find<acre::TransformID>(node_idx);
        if (!node) continue;
        auto trs = node->ptr<acre::TransformID>();
        if (!trs) continue;

        // Compose local affine from stored local TRS
        auto local = acre::math::affine3::identity();
        local *= acre::math::scaling(trs->scale);
        local *= trs->rotation.toAffine();
        local *= acre::math::translation(trs->translation);

        auto world = local;
        if (node->parent)
        {
            auto parent = m_scene->find<acre::TransformID>(node->parent->uuid());
            if (parent)
            {
                world = local * parent->ptr<acre::TransformID>()->affine;
            }
        }

        // Update Transform matrix (world)
        trs->affine = world;
        trs->matrix = acre::math::affineToHomogeneous(world);
        m_scene->update(node);

        auto skin = m_scene->find<acre::SkinID>(node_idx);
        if (skin)
        {
            auto skinptr             = skin->ptr<acre::SkinID>();
            auto inverse_node_matrix = skinptr->inverse_node_matrix;
            auto inverse_bind_matrix = skinptr->inverse_bind_matrix;
            skinptr->joint_matrix    = inverse_bind_matrix * trs->matrix * inverse_node_matrix;
            skinptr->joint_affine    = acre::math::homogeneousToAffine(skinptr->joint_matrix);
            m_scene->update(skin);
        }

        // Propagate to children (compute child's global from child's local * parent_global)
        _update_children(node);
    }
}

void AnimationController::_update_children(acre::Resource* node)
{
    if (!node) return;

    auto node_trs = node->ptr<acre::TransformID>();
    auto children = node->children;
    if (children.empty()) return;

    for (auto child : children)
    {
        if (!child) continue;

        auto child_trs = child->ptr<acre::TransformID>();

        // Compose child's local affine from stored local TRS
        auto local = acre::math::affine3::identity();
        local *= acre::math::scaling(child_trs->scale);
        local *= child_trs->rotation.toAffine();
        local *= acre::math::translation(child_trs->translation);

        // global = local * parent_global (matches loader convention)
        child_trs->affine = local * node_trs->affine;
        child_trs->matrix = acre::math::affineToHomogeneous(child_trs->affine);

        m_scene->update(child);

        auto skin = m_scene->find<acre::SkinID>(child->uuid());
        if (skin)
        {
            auto inverse_node_matrix                = skin->ptr<acre::SkinID>()->inverse_node_matrix;
            auto inverse_bind_matrix                = skin->ptr<acre::SkinID>()->inverse_bind_matrix;
            skin->ptr<acre::SkinID>()->joint_matrix = inverse_bind_matrix * child_trs->matrix * inverse_node_matrix;
            skin->ptr<acre::SkinID>()->joint_affine = acre::math::homogeneousToAffine(skin->ptr<acre::SkinID>()->joint_matrix);
            m_scene->update(skin);
        }

        // Recurse
        _update_children(child);
    }
}
