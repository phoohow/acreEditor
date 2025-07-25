#pragma once

#include <acre/render/scene.h>
#include <model/camera.h>

#include <vector>

class SceneMgr
{
    acre::Scene*                   m_scene;
    std::vector<acre::EntityID>    m_entities;
    std::vector<acre::GeometryID>  m_geometries;
    std::vector<acre::MaterialID>  m_materials;
    std::vector<acre::TransformID> m_transforms;
    std::vector<acre::CameraID>    m_cameras;
    std::vector<acre::LightID>     m_lights;
    std::vector<acre::TextureID>   m_textures;
    std::vector<acre::ImageID>     m_images;
    std::vector<acre::SamplerID>   m_samplers;

    std::vector<acre::TextureID>   m_extTextureList;
    std::vector<acre::ImageID>     m_extImageList;
    std::vector<acre::TransformID> m_extTransforms;

    acre::math::box3 m_box      = acre::math::box3::empty();
    acre::CameraID   m_cameraID = -1;

public:
    SceneMgr(acre::Scene*);

    ~SceneMgr();

    void clearScene();
    void clearHDR();

    acre::UintBufferID   createVIndex(acre::UintBufferPtr);
    acre::Float3BufferID createVPosition(acre::Float3BufferPtr);
    acre::Float2BufferID createVUV(acre::Float2BufferPtr);
    acre::Float3BufferID createVNormal(acre::Float3BufferPtr);
    acre::Float4BufferID createVTangent(acre::Float4BufferPtr);
    acre::Float4BufferID createVColor(acre::Float4BufferPtr);

    acre::ImageID     create(acre::ImagePtr);
    acre::TextureID   create(acre::TexturePtr);
    acre::GeometryID  create(acre::GeometryPtr);
    acre::MaterialID  create(acre::MaterialPtr);
    acre::TransformID create(acre::TransformPtr);
    acre::EntityID    create(acre::EntityPtr);
    void              create(acre::component::DrawPtr);

    acre::ImageID     createExt(acre::ImagePtr);
    acre::TextureID   createExt(acre::TexturePtr);
    acre::TransformID createExt(acre::TransformPtr);

    acre::ImageID     findImage(uint32_t index);
    acre::TextureID   findTexture(uint32_t index);
    acre::GeometryID  findGeometry(uint32_t index);
    acre::MaterialID  findMaterial(uint32_t index);
    acre::TransformID findTransform(uint32_t index);
    acre::EntityID    findEntity(uint32_t index);

    auto getBox() { return m_box; }
    void resetBox() { m_box = acre::math::box3::empty(); }
    void mergeBox(acre::math::box3 box) { m_box |= box; }

    auto getCameraList() { return m_cameras; }
    auto getCameraID() { return m_cameraID; }
    auto getMainCamera() { return m_scene->findCamera(m_cameraID); }
    void setMainCamera(acre::CameraID id) { m_cameraID = id; }

    auto getEntityList() { return m_entities; }
    auto getEntityCount() { return m_entities.size(); }
    void setHighlightEntity(acre::EntityID id) { m_scene->highlightEntity(id); }
    void unhighlightEntity(acre::EntityID id) { m_scene->unhighlightEntity(id); }
    void aliveEntity(acre::EntityID id) { m_scene->findEntity(id)->markAlive(); }
    void unAliveEntity(acre::EntityID id) { m_scene->findEntity(id)->unAlive(); }
    void removeEntity(acre::EntityID id)
    {
        m_entities.erase(std::find(m_entities.begin(), m_entities.end(), id));
        m_scene->removeEntity(id);
    }

    auto getLightList() { return m_lights; }
    auto getLightCount() { return m_lights.size(); }
    auto getLight(acre::LightID id) { return m_scene->findLight(id); }
    void updateLight(acre::LightID id) { m_scene->updateLight(id); }

    auto getSunLight() { return m_scene->getSunLight(); }
    void setHDRLight(acre::HDRLightPtr light) { m_scene->setHDRLight(light); }
    auto getHDRLight() { return m_scene->getHDRLight(); }
    void setLutGGX(acre::TextureID texture) { m_scene->setLutGGX(texture); }
    void setLutCharlie(acre::TextureID texture) { m_scene->setLutCharlie(texture); }
    void setLutSheenAlbedoScale(acre::TextureID texture) { m_scene->setLutSheenAlbedoScale(texture); }

    auto getVIndexBuffer(acre::UintBufferID id) { return m_scene->findVIndex(id); }
    auto getVPositionBuffer(acre::Float3BufferID id) { return m_scene->findVPosition(id); }

    auto getGeometryList() { return m_geometries; }
    auto getGeometryCount() { return m_geometries.size(); }
    auto getGeometry(acre::GeometryID id) { return m_scene->findGeometry(id); }
    void updateGeometry(acre::GeometryID id) { m_scene->updateGeometry(id); }
    void setHighlightGeometry(acre::GeometryID id) { m_scene->highlightGeometry(id); }
    void unhighlightGeometry(acre::GeometryID id) { m_scene->unhighlightGeometry(id); }
    void removeGeometry(acre::GeometryID id)
    {
        m_geometries.erase(std::find(m_geometries.begin(), m_geometries.end(), id));
        m_scene->removeGeometry(id);
    }

    auto getTexture(acre::TextureID id) { return m_scene->findTexture(id); }

    auto getMaterialList() { return m_materials; }
    auto getMaterialCount() { return m_materials.size(); }
    auto getMaterial(acre::MaterialID id) { return m_scene->findMaterial(id); }
    void updateMaterial(acre::MaterialID id) { m_scene->updateMaterial(id); }
    void setHighlightMaterial(acre::MaterialID id) { m_scene->highlightMaterial(id); }
    void unhighlightMaterial(acre::MaterialID id) { m_scene->unhighlightMaterial(id); }
    void removeMaterial(acre::MaterialID id)
    {
        m_materials.erase(std::find(m_materials.begin(), m_materials.end(), id));
        m_scene->removeMaterial(id);
    }

    auto getTransformList() { return m_transforms; }
    auto getTransformCount() { return m_transforms.size(); }
    auto getTransform(acre::TransformID id) { return m_scene->findTransform(id); }
    void updateTransform(acre::TransformID id) { m_scene->updateTransform(id); }

    auto getTextureCount() { return m_textures.size(); }
    auto getImageCount() { return m_images.size(); }

private:
    void init();

    void initCamera();
    void initDirectionLight();
    void initPointLight();
};
