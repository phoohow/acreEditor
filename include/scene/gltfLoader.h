#pragma once

#include "scene/loader.h"

namespace tinygltf
{
class Model;
class TinyGLTF;
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
};
