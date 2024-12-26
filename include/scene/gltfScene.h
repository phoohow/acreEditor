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

    rdr::math::box3 m_box = rdr::math::box3::empty();

public:
    GLTFScene(rdr::Scene*);

    virtual ~GLTFScene() override;

    virtual void createScene() override;

    virtual void registerResource() override;

    void destory();

    virtual void resize(uint32_t width, uint32_t height) override;

    virtual void cameraMove(rdr::math::float3) override;

    virtual void cameraRotateY(float degree);

    virtual void cameraRotateX(float degree);

    virtual void cameraForward();

    virtual void cameraBack();

    virtual void leftView() override;

    virtual void rightView() override;

    virtual void forwardView() override;

    virtual void backView() override;

    virtual void topView() override;

    virtual void bottomView() override;

    virtual void loadGLTF(const std::string& fileName) override;

    virtual void loadHDR(const std::string& fileName) override;

    virtual void saveFrame(const std::string& fileName, rdr::Pixels* pixels) override;

private:
    void createGeometry();

    void createMaterial();

    void createTransform();

    void setCamera();
};
