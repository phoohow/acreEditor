#include <controller/animationController.h>

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

    // Advance time
    {
        m_time += delta_time;
        if (m_time > m_current->duration)
        {
            // m_time = m_current->duration;
            m_time = 0.0f;
        }

        // Sample and cache each channel's animation value (linear interpolation only)
        m_sampled_values.clear();
        for (const auto& channel : m_current->channels)
        {
            const auto& sampler    = m_current->samplers[channel.samplerIndex];
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

    // Apply sampled values to target nodes
    {
        const auto& channels = m_current->channels;
        for (size_t i = 0; i < channels.size(); ++i)
        {
            const auto& channel = channels[i];
            auto        node    = m_scene->find<acre::TransformID>(channel.targetNode);
            if (!node) continue;
            auto transform = node->ptr<acre::TransformID>();
            if (!transform) continue;

            const auto& value  = sampled_value(i);
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
            m_scene->update(node);

            auto skin = m_scene->find<acre::SkinID>(channel.targetNode);
            if (skin)
            {
                auto inverse_node_matrix                = skin->ptr<acre::SkinID>()->inverse_node_matrix;
                auto inverse_bind_matrix                = skin->ptr<acre::SkinID>()->inverse_bind_matrix;
                skin->ptr<acre::SkinID>()->joint_matrix = inverse_bind_matrix * transform->matrix * inverse_node_matrix;
                skin->ptr<acre::SkinID>()->joint_affine = acre::math::homogeneousToAffine(skin->ptr<acre::SkinID>()->joint_matrix);
                m_scene->update(skin);
            }
        }
    }
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
