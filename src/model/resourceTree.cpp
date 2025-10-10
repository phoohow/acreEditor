#pragma once

#include <model/wrapper/resourceTree.h>
#include <acre/render/scene.h>

namespace acre
{

Resource::Resource(UUID u, RID r) :
    uid(u), rid(r)
{
}

Resource::~Resource()
{
}

ResourceTree::ResourceTree(Scene* s) :
    m_scene(s)
{
}

void ResourceTree::_link(Resource* hold, Resource* ref)
{
    hold->refs.emplace(ref);
    ref->holds.emplace(hold);
}

void ResourceTree::_unlink(Resource* hold, Resource* ref)
{
    hold->refs.erase(ref);
    ref->holds.erase(hold);
}

bool ResourceTree::_has(UUID uuid, size_t index)
{
    auto& mgr  = m_mgrs[index];
    auto  iter = mgr.find(uuid);
    return iter != mgr.end();
}

Resource* ResourceTree::_get(UUID uuid, size_t index)
{
    auto& mgr  = m_mgrs[index];
    auto  iter = mgr.find(uuid);
    if (iter != mgr.end())
        return iter->second.get();

    auto rid  = _createID(index);
    auto node = std::unique_ptr<Resource>(new Resource(uuid, rid));
    auto ret  = node.get();
    mgr.emplace(uuid, std::move(node));
    return ret;
}

void ResourceTree::update(Resource* hold, std::unordered_set<Resource*>&& refs)
{
    auto oldRefs = hold->refs;

    hold->refs.clear();
    for (auto ref : refs)
    {
        // erase repeat ref, get un-ref resources
        if (oldRefs.find(ref) != oldRefs.end())
        {
            oldRefs.erase(ref);
        }

        _link(hold, ref);
    }

    for (auto ref : oldRefs)
    {
        _unlink(hold, ref);
        remove(ref);
    }

    updateLeaf(hold);
}

void ResourceTree::updateLeaf(Resource* node)
{
    _updateID(node);

    for (auto& hold : node->holds)
        updateLeaf(hold);
}

void ResourceTree::remove(Resource* node)
{
    if (!node->holds.empty()) return;

    auto& mgr = m_mgrs[node->rid.index()];
    for (auto ref : node->refs)
    {
        _unlink(node, ref);
        remove(ref);
    }

    std::visit([](auto p) { delete p.ptr; }, node->rid);
    _removeID(node->rid);
    mgr.erase(node->uid);
}

void ResourceTree::clear()
{
    auto& mgr = m_mgrs[index_of_rid<EntityID>()];
    while (!mgr.empty())
    {
        auto iter = mgr.begin();
        auto node = iter->second.get();
        remove(node);
    }
}

// clang-format off
RID ResourceTree::_createID(size_t index)
{
    switch (index)
    {
        case index_of_rid<VIndexID>():    {  auto id = VIndexID{new VIndex};       m_scene->create(id); return id; }
        case index_of_rid<VPositionID>(): {  auto id = VPositionID{new VPosition}; m_scene->create(id); return id; }
        case index_of_rid<VUVID>():       {  auto id = VUVID{new VUV};             m_scene->create(id); return id; }
        case index_of_rid<VNormalID>():   {  auto id = VNormalID{new VNormal};     m_scene->create(id); return id; }
        case index_of_rid<VTangentID>():  {  auto id = VTangentID{new VTangent};   m_scene->create(id); return id; }
        case index_of_rid<VColorID>():    {  auto id = VColorID{new VColor};       m_scene->create(id); return id; }
        case index_of_rid<GeometryID>():  {  auto id = GeometryID{new Geometry};   m_scene->create(id); return id; }
        case index_of_rid<ImageID>():     {  auto id = ImageID{new Image};         m_scene->create(id); return id; }
        case index_of_rid<TextureID>():   {  auto id = TextureID{new Texture};     m_scene->create(id); return id; }
        case index_of_rid<SamplerID>():   {  auto id = SamplerID{new Sampler};     m_scene->create(id); return id; }
        case index_of_rid<MaterialID>():  {  auto id = MaterialID{new Material};   m_scene->create(id); return id; }
        case index_of_rid<TransformID>(): {  auto id = TransformID{new Transform}; m_scene->create(id); return id; }
        case index_of_rid<EntityID>():    {  auto id = EntityID{new Entity};       m_scene->create(id); return id; }
        case index_of_rid<LightID>():     {  auto id = LightID{new Light};         m_scene->create(id); return id; }
        case index_of_rid<CameraID>():    {  auto id = CameraID{new Camera};       m_scene->create(id); return id; }
    }

    return EntityID();
}

void ResourceTree::_updateID(Resource* node)
{
    auto vistor = [this](auto id)
    {
        m_scene->update(id);
    };
    std::visit(std::move(vistor), node->rid);

    // auto rid = node->rid;
    // switch (rid.index())
    // {
    //     case index_of_rid<VIndexID>():    m_scene->update(std::get<VIndexID>(rid));  break;
    //     case index_of_rid<VPositionID>(): m_scene->update(std::get<VPositionID>(rid)); break;
    //     case index_of_rid<VUVID>():       m_scene->update(std::get<VUVID>(rid));       break;
    //     case index_of_rid<VNormalID>():   m_scene->update(std::get<VNormalID>(rid));   break;
    //     case index_of_rid<VTangentID>():  m_scene->update(std::get<VTangentID>(rid));  break;
    //     case index_of_rid<VColorID>():    m_scene->update(std::get<VColorID>(rid));    break;
    //     case index_of_rid<GeometryID>():  m_scene->update(std::get<GeometryID>(rid));  break;
    //     case index_of_rid<ImageID>():     m_scene->update(std::get<ImageID>(rid));     break;
    //     case index_of_rid<TextureID>():   m_scene->update(std::get<TextureID>(rid));   break;
    //     case index_of_rid<SamplerID>():   m_scene->update(std::get<SamplerID>(rid));   break;
    //     case index_of_rid<MaterialID>():  m_scene->update(std::get<MaterialID>(rid));  break;
    //     case index_of_rid<TransformID>(): m_scene->update(std::get<TransformID>(rid)); break;
    //     case index_of_rid<EntityID>():    m_scene->update(std::get<EntityID>(rid));    break;
    //     case index_of_rid<LightID>():     m_scene->update(std::get<LightID>(rid));     break;
    //     case index_of_rid<CameraID>():    m_scene->update(std::get<CameraID>(rid));    break;
    // }
}

void ResourceTree::_removeID(RID rid)
{
    auto vistor = [this](auto id)
    {
        m_scene->remove(id);
        delete id.ptr;
    };
    std::visit(std::move(vistor), rid);

    // switch (rid.index())
    // {
    //     case index_of_rid<VIndexID>():    { auto id = std::get<VIndexID>(rid);    m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<VPositionID>(): { auto id = std::get<VPositionID>(rid); m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<VUVID>():       { auto id = std::get<VUVID>(rid);       m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<VNormalID>():   { auto id = std::get<VNormalID>(rid);   m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<VTangentID>():  { auto id = std::get<VTangentID>(rid);  m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<VColorID>():    { auto id = std::get<VColorID>(rid);    m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<GeometryID>():  { auto id = std::get<GeometryID>(rid);  m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<ImageID>():     { auto id = std::get<ImageID>(rid);     m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<TextureID>():   { auto id = std::get<TextureID>(rid);   m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<SamplerID>():   { auto id = std::get<SamplerID>(rid);   m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<MaterialID>():  { auto id = std::get<MaterialID>(rid);  m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<TransformID>(): { auto id = std::get<TransformID>(rid); m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<EntityID>():    { auto id = std::get<EntityID>(rid);    m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<LightID>():     { auto id = std::get<LightID>(rid);     m_scene->remove(id); delete id.ptr; break; }
    //     case index_of_rid<CameraID>():    { auto id = std::get<CameraID>(rid);    m_scene->remove(id); delete id.ptr; break; }
    // }
}

// clang-format on

} // namespace acre
