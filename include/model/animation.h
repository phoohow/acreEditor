#pragma once
#include <vector>
#include <string>

namespace acre
{

struct AnimationChannel
{
    int         targetNode;
    std::string targetPath; // "translation", "rotation", "scale"
    int         samplerIndex;
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
    Animation*             getAnimation(const std::string& name);
};

} // namespace acre
