#pragma once
#include <Render/scene/scene.h>
#include <vector>

namespace rdr
{
struct Pixels;
}

class BaseScene
{
protected:
    rdr::Scene*                   m_scene;
    std::vector<rdr::EntityID>    m_entities;
    std::vector<rdr::GeometryID>  m_geometrys;
    std::vector<rdr::MaterialID>  m_materials;
    std::vector<rdr::TransformID> m_transforms;
    std::vector<rdr::CameraID>    m_cameras;
    std::vector<rdr::LightID>     m_lights;
    std::vector<rdr::TextureID>   m_textures;
    std::vector<rdr::ImageID>     m_images;
    std::vector<rdr::SamplerID>   m_samplers;

    std::vector<rdr::TextureID> m_textureExts;
    std::vector<rdr::ImageID>   m_imageExts;
    std::vector<rdr::LightID>   m_lightExts;

    uint32_t m_width  = 1;
    uint32_t m_height = 1;

public:
    BaseScene(rdr::Scene*);

    virtual ~BaseScene();

    virtual void createScene() = 0;

    virtual void registerResource() = 0;

    virtual void resize(uint32_t width, uint32_t height) {}

    virtual void cameraMove(rdr::math::float3) {}
    virtual void cameraRotateY(float degree) {}
    virtual void cameraRotateX(float degree) {}
    virtual void cameraForward() {}
    virtual void cameraBack() {}

    virtual void leftView() {}
    virtual void rightView() {}
    virtual void forwardView() {}
    virtual void backView() {}
    virtual void topView() {}
    virtual void bottomView() {}

    virtual void loadGLTF(const std::string& fileName) {}
    void         clearScene();
    virtual void loadHDR(const std::string& fileName) {}
    void         clearHDR();
    virtual void saveFrame(const std::string& fileName, rdr::Pixels* pixels) {}

    auto getCameras() { return m_cameras; }
    auto getCamera() { return m_scene->getMainCamera(); }
    void setMainCamera(rdr::CameraID cameraID) { m_scene->setMainCamera(cameraID); }

    auto getEntityCount() { return m_entities.size(); }

    auto getLights() { return m_lights; }
    auto getLightCount() { return m_lights.size(); }
    auto getLight(rdr::LightID id) { return m_scene->findLight(id); }
    auto getHDRLight() { return m_scene->getHDRLight(); }
    void updateLight(rdr::LightID id) { m_scene->updateLight(id); }

    auto getGeometrys() { return m_geometrys; }
    auto getGeometryCount() { return m_geometrys.size(); }
    auto getGeometry(rdr::GeometryID id) { return m_scene->findGeometry(id); }
    void updateGeometry(rdr::GeometryID id) { m_scene->updateGeometry(id); }
    void setHighlightGeometry(rdr::GeometryID id) { m_scene->highlightGeometry(id); }
    void unhighlightGeometry(rdr::GeometryID id) { m_scene->unhighlightGeometry(id); }

    auto getMaterials() { return m_materials; }
    auto getMaterialCount() { return m_materials.size(); }
    auto getMaterial(rdr::MaterialID id) { return m_scene->findMaterial(id); }
    void updateMaterial(rdr::MaterialID id) { m_scene->updateMaterial(id); }
    void setHighlightMaterial(rdr::MaterialID id) { m_scene->highlightMaterial(id); }
    void unhighlightMaterial(rdr::MaterialID id) { m_scene->unhighlightMaterial(id); }

    auto getTransforms() { return m_transforms; }
    auto getTransformCount() { return m_transforms.size(); }
    auto getTransform(rdr::TransformID id) { return m_scene->findTransform(id); }
    void updateTransform(rdr::TransformID id) { m_scene->updateTransform(id); }

    auto getTextureCount() { return m_textures.size(); }
    auto getImageCount() { return m_images.size(); }

private:
    void init();

    void createDirectionLight();
    void createPointLight();
    void createCamera();
};
