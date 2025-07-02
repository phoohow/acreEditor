#include <scene/sceneMgr.h>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <acre/utils/math/math.h>
#include <acre/render/renderer.h>

static constexpr double M_PI  = 3.14159265358979;
static constexpr float  g_fov = 60;

static void setCameraView(Camera* camera, acre::math::box3 box, acre::math::float3 dir)
{
    auto fov      = g_fov;
    auto radius   = acre::math::length(box.diagonal()) * 0.5f;
    auto distance = radius / sinf(acre::math::radians(fov * 0.5f));
    camera->setPosition(box.center() + dir * distance);
    camera->setTarget(box.center());
    camera->resetYaw();
    camera->resetPitch();
}

SceneMgr::SceneMgr(acre::Scene* scene) :
    m_scene(scene)
{
    initCamera();
    initDirectionLight();
    // initPointLight();
}

SceneMgr::~SceneMgr()
{
}

void SceneMgr::initCamera()
{
    m_camera = new Camera;

    auto mainCamera = std::make_shared<acre::Camera>();
    m_cameraID      = m_scene->create(mainCamera);
    m_cameras.emplace_back(m_cameraID);

    m_camera->setPosition(acre::math::float3(0.0f, 0.0f, 1.0f));
    m_camera->setTarget(0.0f);
    m_camera->setUp(acre::math::float3(0.0f, 1.0f, 0.0f));

    m_camera->setFOV(45.0f);
    m_camera->setAspect(1.0f);
    m_camera->setNear(1e-1f);
    m_camera->setFar(1e6f);

    syncCamera();
}

void SceneMgr::initDirectionLight()
{
    auto light       = std::make_shared<acre::SunLight>();
    light->color     = acre::math::float3(1.0, 1.0, 1.0);
    light->direction = acre::math::normalize(acre::math::float3(0, -1, -1));
    light->factor    = 1.0;
    light->enable    = true;
    m_scene->setSunLight(light);
}

void SceneMgr::initPointLight()
{
    acre::PointLight point;
    point.position = acre::math::float3(0, 1, 1);

    auto light   = std::make_shared<acre::Light>();
    light->light = point;
    light->type  = acre::LightType::Point;
    m_lights.emplace_back(m_scene->create(light));
}

void SceneMgr::updateBox(acre::math::box3 box)
{
    m_box |= box;
    resetMainCamera();
}

void SceneMgr::resetMainCamera()
{
    auto fov    = g_fov;
    auto radius = acre::math::length(m_box.diagonal()) * 0.5f;

    auto mainCamera = getMainCamera();
    if (mainCamera->type == acre::Camera::ProjectType::tPerspective)
    {
        m_camera->setFOV(fov);
        m_camera->setAspect(float(m_width) / float(m_height));
        m_camera->setNear(radius * 0.1);
        m_camera->setFar(100000.0f);
    }
    else
    {
        m_camera->setNear(radius * 0.1);
        m_camera->setFar(100000.0f);
        m_camera->setLeft(-radius);
        m_camera->setRight(radius);
        m_camera->setTop(radius);
        m_camera->setBottom(-radius);
    }

    forwardView();
}

void SceneMgr::syncCamera()
{
    auto mainCamera      = getMainCamera();
    mainCamera->position = m_camera->getPosition();
    mainCamera->target   = m_camera->getTarget();
    mainCamera->up       = m_camera->getUp();

    if (mainCamera->type == acre::Camera::ProjectType::tPerspective)
    {
        auto& projection     = std::get<acre::Camera::Perspective>(mainCamera->projection);
        projection.fov       = m_camera->getFOV();
        projection.aspect    = m_camera->getAspect();
        projection.nearPlane = m_camera->getNear();
        projection.farPlane  = m_camera->getFar();
    }
    else
    {
        auto& projection       = std::get<acre::Camera::Orthonormal>(mainCamera->projection);
        projection.nearPlane   = m_camera->getNear();
        projection.farPlane    = m_camera->getFar();
        projection.topPlane    = m_camera->getTop();
        projection.bottomPlane = m_camera->getBottom();
        projection.leftPlane   = m_camera->getLeft();
        projection.rightPlane  = m_camera->getRight();
    }
}

void SceneMgr::clearScene()
{
    for (auto id : m_entities)
        m_scene->removeEntity(id);
    m_entities.resize(0);

    for (auto id : m_geometries)
        m_scene->removeGeometry(id);
    m_geometries.resize(0);

    for (auto id : m_materials)
        m_scene->removeMaterial(id);
    m_materials.resize(0);

    for (auto id : m_transforms)
        m_scene->removeTransform(id);
    m_transforms.resize(0);

    for (auto id : m_textures)
        m_scene->removeTexture(id);
    m_textures.resize(0);

    for (auto id : m_images)
        m_scene->removeImage(id);
    m_images.resize(0);

    for (auto id : m_samplers)
        m_scene->removeSampler(id);
    m_samplers.resize(0);
}

void SceneMgr::clearHDR()
{
    for (auto index : m_hdrImages)
        m_scene->removeImage(index);
    m_hdrImages.resize(0);

    for (auto index : m_hdrTextures)
        m_scene->removeTexture(index);
    m_hdrTextures.resize(0);
}

void SceneMgr::resize(uint32_t width, uint32_t height)
{
    m_width  = width;
    m_height = height;

    auto fov    = g_fov;
    auto radius = acre::math::length(m_box.diagonal()) * 0.5f;

    auto mainCamera = getMainCamera();
    if (mainCamera->type == acre::Camera::ProjectType::tPerspective)
    {
        m_camera->setFOV(fov);
        m_camera->setAspect(float(m_width) / float(m_height));
        m_camera->setNear(radius * 0.1);
        m_camera->setFar(100000.0f);
    }
    else
    {
        m_camera->setNear(radius * 0.1);
        m_camera->setFar(100000.0f);
        m_camera->setLeft(-radius);
        m_camera->setRight(radius);
        m_camera->setTop(radius);
        m_camera->setBottom(-radius);
    }

    syncCamera();
}

void SceneMgr::cameraMove(acre::math::float3 delta)
{
    auto radius = acre::math::length(m_box.diagonal()) * 0.5f;
    m_camera->translate(delta * radius * 0.02f);

    syncCamera();
}

void SceneMgr::cameraForward()
{
    auto dir = acre::math::normalize(m_camera->getTarget() - m_camera->getPosition());
    cameraMove(dir);
}

void SceneMgr::cameraBack()
{
    auto dir = acre::math::normalize(m_camera->getTarget() - m_camera->getPosition());
    cameraMove(-dir);
}

void SceneMgr::cameraRotateY(float degree)
{
    m_camera->rotate(0, acre::math::radians(degree));

    syncCamera();
}

void SceneMgr::cameraRotateX(float degree)
{
    m_camera->rotate(acre::math::radians(degree), 0);

    syncCamera();
}

void SceneMgr::leftView()
{
    setCameraView(m_camera, m_box, acre::math::float3(1, 0, 0));
    m_camera->rotate(0, M_PI * 1.5);

    syncCamera();
}

void SceneMgr::rightView()
{
    setCameraView(m_camera, m_box, acre::math::float3(-1, 0, 0));
    m_camera->rotate(0, M_PI * 0.5);

    syncCamera();
}

void SceneMgr::forwardView()
{
    setCameraView(m_camera, m_box, acre::math::float3(0, 0, 1));
    m_camera->rotate(0, M_PI);

    syncCamera();
}

void SceneMgr::backView()
{
    setCameraView(m_camera, m_box, acre::math::float3(0, 0, -1));
    m_camera->rotate(0, 0);

    syncCamera();
}

void SceneMgr::topView()
{
    setCameraView(m_camera, m_box, acre::math::float3(0, 1, 0));
    m_camera->rotate(M_PI * 0.5, 0);

    syncCamera();
}

void SceneMgr::bottomView()
{
    setCameraView(m_camera, m_box, acre::math::float3(0, -1, 0));
    m_camera->rotate(-M_PI * 0.5, 0);

    syncCamera();
}

void SceneMgr::saveFrame(const std::string& fileName, acre::Pixels* pixels)
{
    stbi_write_bmp(fileName.c_str(), pixels->desc.width, pixels->desc.height, 4, (void*)pixels->data);
}

acre::UintBufferID SceneMgr::createVIndexBuffer(acre::UintBufferPtr buffer)
{
    return m_scene->createVIndexBuffer(buffer);
}

acre::Float3BufferID SceneMgr::createVPositionBuffer(acre::Float3BufferPtr buffer)
{
    return m_scene->createVPositionBuffer(buffer);
}

acre::Float2BufferID SceneMgr::createVUVBuffer(acre::Float2BufferPtr buffer)
{
    return m_scene->createVUVBuffer(buffer);
}

acre::Float3BufferID SceneMgr::createVNormalBuffer(acre::Float3BufferPtr buffer)
{
    return m_scene->createVNormalBuffer(buffer);
}

acre::Float4BufferID SceneMgr::createVTangentBuffer(acre::Float4BufferPtr buffer)
{
    return m_scene->createVTangentBuffer(buffer);
}

acre::ImageID SceneMgr::create(acre::ImagePtr image)
{
    auto id = m_scene->create(image);
    m_images.push_back(id);
    return id;
}

acre::TextureID SceneMgr::create(acre::TexturePtr texture)
{
    auto id = m_scene->create(texture);
    m_textures.push_back(id);
    return id;
}

acre::GeometryID SceneMgr::create(acre::GeometryPtr geometry)
{
    auto id = m_scene->create(geometry);
    m_geometries.push_back(id);
    return id;
}

acre::MaterialID SceneMgr::create(acre::MaterialPtr material)
{
    auto id = m_scene->create(material);
    m_materials.push_back(id);
    return id;
}

acre::TransformID SceneMgr::create(acre::TransformPtr transform)
{
    auto id = m_scene->create(transform);
    m_transforms.push_back(id);
    return id;
}

acre::EntityID SceneMgr::create(acre::EntityPtr entity)
{
    auto id = m_scene->create(entity);
    m_entities.push_back(id);
    return id;
}

void SceneMgr::create(acre::component::DrawPtr draw)
{
    m_scene->createComponentDraw(draw);
}

acre::ImageID SceneMgr::findImage(uint32_t index)
{
    if (index < m_images.size())
    {
        return m_images[index];
    }

    return RESOURCE_ID_VALID;
}

acre::TextureID SceneMgr::findTexture(uint32_t index)
{
    if (index < m_textures.size())
    {
        return m_textures[index];
    }

    return RESOURCE_ID_VALID;
}

acre::GeometryID SceneMgr::findGeometry(uint32_t index)
{
    if (index < m_geometries.size())
    {
        return m_geometries[index];
    }

    return RESOURCE_ID_VALID;
}

acre::MaterialID SceneMgr::findMaterial(uint32_t index)
{
    if (index < m_materials.size())
    {
        return m_materials[index];
    }

    return RESOURCE_ID_VALID;
}

acre::TransformID SceneMgr::findTransform(uint32_t index)
{
    if (index < m_transforms.size())
    {
        return m_transforms[index];
    }

    return RESOURCE_ID_VALID;
}

acre::EntityID SceneMgr::findEntity(uint32_t index)
{
    if (index < m_entities.size())
    {
        return m_entities[index];
    }

    return RESOURCE_ID_VALID;
}
