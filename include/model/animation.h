#pragma once
#include <vector>
#include <string>

namespace acre
{

struct AnimationChannel
{
    int         target_node;
    std::string target_path; // "translation", "rotation", "scale"
    int         sampler_idx;
};

struct AnimationSampler
{
    std::vector<float>              input;         // keyframe times
    std::vector<std::vector<float>> output;        // keyframe values
    std::string                     interpolation; // "LINEAR", "STEP", "CUBICSPLINE"
};

struct Animation
{
    std::string                   name;
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
    float                         duration = 0.0f;
};

class AnimationSet
{
public:
    std::vector<Animation> animations;

    Animation* animation(const std::string& name);
    Animation* animation(size_t index) { return &animations[index]; }
};

} // namespace acre
