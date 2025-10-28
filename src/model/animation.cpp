#include <model/animation.h>

namespace acre
{

Animation* AnimationSet::getAnimation(const std::string& name)
{
    for (auto& animation : animations)
    {
        if (animation.name == name) return &animation;
    }

    return nullptr;
}

} // namespace acre
