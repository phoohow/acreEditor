#pragma once

#include <acre/render/scene.h>

#include <unordered_set>
#include <memory>
#include <variant>

namespace acre
{

using RID = std::variant<VIndexID,
                         VPositionID,
                         VUVID,
                         VNormalID,
                         VTangentID,
                         VColorID,
                         VJointID,
                         VWeightID,
                         GeometryID,
                         ImageID,
                         SamplerID,
                         TextureID,
                         TransformID,
                         MaterialID,
                         EntityID,
                         LightID,
                         CameraID,
                         SkinID>;

template <typename T, size_t N = std::variant_size_v<RID> - 1>
constexpr auto is_one_of_rid()
{
    using type           = std::variant_alternative_t<N, RID>;
    constexpr auto value = std::is_same_v<T, type>;
    if constexpr (value || N == 0)
    {
        return value;
    }
    else
    {
        return is_one_of_rid<T, N - 1>();
    }
}

template <typename ID>
constexpr auto index_of_rid()
{
    static_assert(is_one_of_rid<ID>(), "T should be one of RID");

    return RID((ID)0).index();
}

using UUID = uint32_t;

struct Resource
{
    UUID uuid() const { return uid; }

    uint32_t idx() const
    {
        return std::visit([](auto p) { return p.idx; }, rid);
    }

    template <typename ID>
    ID id() const { return std::get<ID>(rid); }

    template <typename ID>
    auto ptr() const { return std::get<ID>(rid).ptr; }

private:
    friend class ResourceTree;

    UUID uid;
    RID  rid;

    // ref tree
    std::unordered_set<Resource*> holds;
    std::unordered_set<Resource*> refs;

    Resource(UUID u, RID r);

public:
    ~Resource();
};

} // namespace acre