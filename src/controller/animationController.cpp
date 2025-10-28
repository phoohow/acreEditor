#include <controller/animationController.h>

AnimationController::AnimationController(acre::AnimationSet* set) :
    m_set(set), m_current(nullptr), m_time(0.0f), m_playing(false)
{}

void AnimationController::play(const std::string& name)
{
    m_current = m_set->getAnimation(name);
    m_time    = 0.0f;
    m_playing = (m_current != nullptr);
}

void AnimationController::pause()
{
    m_playing = false;
}

void AnimationController::stop()
{
    m_playing = false;
    m_time    = 0.0f;
}

void AnimationController::update(float deltaTime)
{
    if (m_playing && m_current)
    {
        m_time += deltaTime;
        if (m_time > m_current->duration)
        {
            m_time    = m_current->duration;
            m_playing = false;
        }

        // Sample and cache each channel's animation value (linear interpolation only)
        m_sampledValues.clear();
        for (const auto& channel : m_current->channels)
        {
            const auto& sampler    = m_current->samplers[channel.samplerIndex];
            size_t      frameCount = sampler.input.size();
            if (frameCount < 2)
            {
                m_sampledValues.push_back({});
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

            m_sampledValues.push_back(value);
        }
    }
}

const std::vector<float>& AnimationController::getSampledValue(size_t channelIndex) const
{
    static const std::vector<float> empty;
    if (channelIndex < m_sampledValues.size())
        return m_sampledValues[channelIndex];
    return empty;
}

float AnimationController::getCurrentTime() const
{
    return m_time;
}

bool AnimationController::isPlaying() const
{
    return m_playing;
}
