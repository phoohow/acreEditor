#pragma once

#include "resource.h"
#include <unordered_map>

class SceneMgr;
namespace acre
{

class Scene;
class ResourceTree
{
    friend class SceneMgr;

    Scene* m_scene;

    using ResourceMgr = std::unordered_map<UUID, std::unique_ptr<Resource>>;
    std::vector<ResourceMgr> m_mgrs{std::variant_size_v<RID>};

public:
    ResourceTree(Scene*);

    template <typename T>
    Resource* get(UUID uuid) { return _get(uuid, index_of_rid<T>()); }

    template <typename T>
    bool has(UUID uuid) { return _has(uuid, index_of_rid<T>()); }

    void update(Resource* hold, std::unordered_set<Resource*>&& refs);

    void incRefs(Resource* hold, std::unordered_set<Resource*>&& refs);

    void updateLeaf(Resource* hold);

    void remove(Resource* node);

    void clear();

private:
    void _link(Resource* hold, Resource* ref);

    void _unlink(Resource* hold, Resource* ref);

    Resource* _get(UUID uuid, size_t index);

    bool _has(UUID uuid, size_t index);

    RID _createID(size_t index);

    void _updateID(Resource* node);

    void _removeID(RID rid);

    template <typename T>
    auto& _getMgr() { return m_mgrs[index_of_rid<T>()]; }
};

} // namespace acre
