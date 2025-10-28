#pragma once

#include <model/animation.h>

#include <string>

class AnimationController
{
    acre::AnimationSet* m_set;
    acre::Animation*    m_current;
    float               m_time;
    bool                m_playing;
    // Sampled results for each channel
    std::vector<std::vector<float>> m_sampledValues;

public:
    auto getAnimationSet() const { return m_set; }
    AnimationController(acre::AnimationSet* set);
    void  play(const std::string& name);
    void  pause();
    void  stop();
    void  update(float deltaTime);
    float getCurrentTime() const;
    bool  isPlaying() const;

    // Get sampled value for a channel
    const std::vector<float>& getSampledValue(size_t channelIndex) const;

    // Get current animation
    const auto getCurrentAnimation() const { return m_current; }
};
