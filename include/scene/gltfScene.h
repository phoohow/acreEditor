#pragma once
#include <scene/baseScene.h>
#include <vector>

namespace tinygltf
{
class Model;
class TinyGLTF;
} // namespace tinygltf

class GLTFScene : public BaseScene
{
    tinygltf::Model*    m_model  = nullptr;
    tinygltf::TinyGLTF* m_loader = nullptr;

public:
    GLTFScene(acre::Scene*);

    virtual ~GLTFScene() override;

    void destory();

    virtual void createScene() override;

    virtual void registerResource() override;

    virtual void loadGLTF(const std::string& fileName) override;

    virtual void loadHDR(const std::string& fileName) override;

    virtual void saveFrame(const std::string& fileName, acre::Pixels* pixels) override;

private:
    void createGeometry();

    void createMaterial();

    void createTransform();
};
