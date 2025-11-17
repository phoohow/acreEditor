#pragma once

#include <model/animation.h>
#include <model/sceneMgr.h>

#include <string>

class AnimationController
{
    SceneMgr* m_scene = nullptr;

    acre::Animation* m_current = nullptr;
    float            m_time    = 0.0f;

    std::vector<std::vector<float>> m_sampled_values;

public:
    AnimationController(SceneMgr* scene);

    void play(const std::string& name);

    void stop();

    void update(float delta_time);

    auto current_time() const { return m_time; }

    const auto current_animation() const { return m_current; }

    const std::vector<float>& sampled_value(size_t channel_idx) const;

private:
    void _try_play();

    void _update_sampled_values();

    void _update_scene();

    void _update_children(acre::Resource* node);
};
