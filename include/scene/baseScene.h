#pragma once

#include <acre/render/scene.h>
#include "camera.h"
#include <vector>

namespace acre
{
struct Pixels;
}

class BaseScene
{
protected:
    acre::Scene*                   m_scene;
    std::vector<acre::EntityID>    m_entities;
    std::vector<acre::GeometryID>  m_geometrys;
    std::vector<acre::MaterialID>  m_materials;
    std::vector<acre::TransformID> m_transforms;
    std::vector<acre::CameraID>    m_cameras;
    std::vector<acre::LightID>     m_lights;
    std::vector<acre::TextureID>   m_textures;
    std::vector<acre::ImageID>     m_images;
    std::vector<acre::SamplerID>   m_samplers;

    std::vector<acre::TextureID> m_textureExts;
    std::vector<acre::ImageID>   m_imageExts;

    acre::math::box3 m_box      = acre::math::box3::empty();
    acre::CameraID   m_cameraID = -1;
    Camera*          m_camera   = nullptr;

    uint32_t m_width  = 1;
    uint32_t m_height = 1;

public:
    BaseScene(acre::Scene*);

    virtual ~BaseScene();

    virtual void createScene() = 0;

    virtual void registerResource() = 0;

    void resize(uint32_t width, uint32_t height);

    void setMainCamera();
    void cameraMove(acre::math::float3);
    void cameraRotateY(float degree);
    void cameraRotateX(float degree);
    void cameraForward();
    void cameraBack();

    void leftView();
    void rightView();
    void forwardView();
    void backView();
    void topView();
    void bottomView();

    virtual void loadGLTF(const std::string& fileName) {}
    void         clearScene();
    virtual void loadHDR(const std::string& fileName) {}
    void         clearHDR();
    virtual void saveFrame(const std::string& fileName, acre::Pixels* pixels) {}

    auto getCameras() { return m_cameras; }
    auto getCameraID() { return m_cameraID; }
    auto getMainCamera() { return m_scene->findCamera(m_cameraID); }
    void setMainCamera(acre::CameraID id) { m_cameraID = id; }

    auto getEntityCount() { return m_entities.size(); }

    auto getLights() { return m_lights; }
    auto getLightCount() { return m_lights.size(); }
    auto getLight(acre::LightID id) { return m_scene->findLight(id); }
    auto getHDRLight() { return m_scene->getHDRLight(); }
    auto getSunLight() { return m_scene->getSunLight(); }
    void updateLight(acre::LightID id) { m_scene->updateLight(id); }

    auto getGeometrys() { return m_geometrys; }
    auto getGeometryCount() { return m_geometrys.size(); }
    auto getGeometry(acre::GeometryID id) { return m_scene->findGeometry(id); }
    void updateGeometry(acre::GeometryID id) { m_scene->updateGeometry(id); }
    void setHighlightGeometry(acre::GeometryID id) { m_scene->highlightGeometry(id); }
    void unhighlightGeometry(acre::GeometryID id) { m_scene->unhighlightGeometry(id); }

    auto getMaterials() { return m_materials; }
    auto getMaterialCount() { return m_materials.size(); }
    auto getMaterial(acre::MaterialID id) { return m_scene->findMaterial(id); }
    void updateMaterial(acre::MaterialID id) { m_scene->updateMaterial(id); }
    void setHighlightMaterial(acre::MaterialID id) { m_scene->highlightMaterial(id); }
    void unhighlightMaterial(acre::MaterialID id) { m_scene->unhighlightMaterial(id); }

    auto getTransforms() { return m_transforms; }
    auto getTransformCount() { return m_transforms.size(); }
    auto getTransform(acre::TransformID id) { return m_scene->findTransform(id); }
    void updateTransform(acre::TransformID id) { m_scene->updateTransform(id); }

    auto getTextureCount() { return m_textures.size(); }
    auto getImageCount() { return m_images.size(); }

protected:
    void swapCamera();

private:
    void init();

    void createDirectionLight();
    void createPointLight();
    void createCamera();
};
