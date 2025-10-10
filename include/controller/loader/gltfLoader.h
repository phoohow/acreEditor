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

    void createComponent();

    void createTextureTransform(const std::map<std::string, tinygltf::Value>& map, acre::TextureID textureID);

    void checkCreateTextureTransform(const tinygltf::Value& value, acre::TextureID textureID);

    void createTextureTransform(const tinygltf::Value& value, acre::TextureID textureID);

    acre::TextureID   _get_texture(uint32_t uuid);
    acre::TransformID _get_transform(uint32_t uuid);
    acre::GeometryID  _get_geometry(uint32_t uuid);
    acre::MaterialID  _get_material(uint32_t uuid);
};
