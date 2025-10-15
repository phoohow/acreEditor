#pragma once

#include <controller/loader.h>

#include <map>

namespace tinygltf
{
class Model;
class TinyGLTF;
class Value;
} // namespace tinygltf

class GLTFLoader : public Loader
{
    tinygltf::Model*    m_model  = nullptr;
    tinygltf::TinyGLTF* m_loader = nullptr;

public:
    GLTFLoader(SceneMgr*);

    virtual ~GLTFLoader() override;

    virtual void loadScene(const std::string& fileName) override;

private:
    void createGeometry();

    void createMaterial();

    void createTransform();

    void create_component_draw();

    void createTextureTransform(const std::map<std::string, tinygltf::Value>& map, uint32_t uuid);

    void checkCreateTextureTransform(const tinygltf::Value& value, uint32_t uuid);

    void createTextureTransform(const tinygltf::Value& value, uint32_t uuid);

    acre::Resource* _get_image(std::unordered_set<acre::Resource*>& refs, uint32_t uuid);
    acre::Resource* _get_texture(std::unordered_set<acre::Resource*>& refs, uint32_t uuid);
    acre::Resource* _get_transform(uint32_t uuid);
    acre::Resource* _get_geometry(uint32_t uuid);
    acre::Resource* _get_material(uint32_t uuid);

    acre::ImageID     _get_image_id(std::unordered_set<acre::Resource*>& refs, uint32_t uuid);
    acre::TextureID   _get_texture_id(std::unordered_set<acre::Resource*>& refs, uint32_t uuid);
    acre::TransformID _get_transform_id(uint32_t uuid);
    acre::GeometryID  _get_geometry_id(uint32_t uuid);
    acre::MaterialID  _get_material_id(uint32_t uuid);
};
