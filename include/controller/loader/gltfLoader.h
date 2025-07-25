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
};
