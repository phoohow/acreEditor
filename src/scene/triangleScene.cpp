#include <scene/triangleScene.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <config.h>
#include <string>

constexpr float g_position[4][3] = {
    {-1.0, -1.0, 0.0},
    {-1.0, 1.0, 0.0},
    {1.0, 1.0, 0.0},
    {1.0, -1.0, 0.0}};

constexpr float g_uv[4][2] = {
    {0.0, 0.0},
    {0.0, 1.0},
    {1.0, 1.0},
    {1.0, 0.0}};

constexpr uint32_t g_index[] = {
    0, 1, 2,
    0, 2, 3};

uint8_t g_image[][4] = {
    {0, 0, 255, 255},
    {0, 255, 0, 255},
    {255, 0, 0, 255},
    {255, 255, 255, 255}};

TriangleScene::TriangleScene(rdr::Scene* scene) :
    BaseScene(scene)
{
}

TriangleScene::~TriangleScene()
{
}

void TriangleScene::createScene()
{
    createEntity();
    createMaterial();
    createGeometry();
    createTransform();
    createCamera();
    registerResource();
}

void TriangleScene::createEntity()
{
    m_entities.emplace_back(m_scene->create(std::make_shared<rdr::Entity>("0")));
    m_entities.emplace_back(m_scene->create(std::make_shared<rdr::Entity>("1")));
    m_entities.emplace_back(m_scene->create(std::make_shared<rdr::Entity>("2")));
}

void TriangleScene::createGeometry()
{
    auto geometry          = std::make_shared<rdr::Geometry>();
    geometry->indexData    = (void*)g_index;
    geometry->positionData = (void*)g_position;
    geometry->uvData       = (void*)g_uv;
    geometry->indexCount   = 6;
    geometry->vertexCount  = 4;
    m_geometrys.emplace_back(m_scene->create(geometry));

    geometry               = std::make_shared<rdr::Geometry>();
    geometry->indexData    = (void*)g_index;
    geometry->positionData = (void*)g_position;
    geometry->uvData       = (void*)g_uv;
    geometry->indexCount   = 6;
    geometry->vertexCount  = 4;
    m_geometrys.emplace_back(m_scene->create(geometry));
}

void TriangleScene::createMaterial()
{
    auto image    = std::make_shared<rdr::Image>();
    image->data   = (void*)g_image;
    image->width  = 2;
    image->height = 2;
    image->format = rdr::ImageFormat::RGBA8;
    auto imageID  = m_scene->create(image);
    m_images.emplace_back(imageID);
    auto texture   = std::make_shared<rdr::Texture>();
    texture->image = imageID;
    m_textures.emplace_back(m_scene->create(texture));

    std::string imgPath     = SRC_DIR "/res/img/desert.png";
    int         imgWidth    = 0;
    int         imgHeight   = 0;
    int         imgChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imgData = stbi_load(imgPath.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);

    image         = std::make_shared<rdr::Image>();
    image->data   = (void*)imgData;
    image->width  = imgWidth;
    image->height = imgHeight;
    image->format = rdr::ImageFormat::RGBA8;
    imageID       = m_scene->create(image);
    m_images.emplace_back(imageID);
    texture        = std::make_shared<rdr::Texture>();
    texture->image = imageID;
    m_textures.emplace_back(m_scene->create(texture));

    auto material            = std::make_shared<rdr::Material>();
    material->baseColor      = {1.0f, 0.0f, 0.0f, 1.0f};
    material->baseColorIndex = m_textures[0];
    m_materials.emplace_back(m_scene->create(material));

    material                 = std::make_shared<rdr::Material>();
    material->baseColor      = {0.0f, 1.0f, 0.0f, 1.0f};
    material->baseColorIndex = m_textures[1];
    m_materials.emplace_back(m_scene->create(material));
}

void TriangleScene::createTransform()
{
    auto transform       = std::make_shared<rdr::Transform>();
    transform->transform = rdr::math::float4x4::identity();
    m_transforms.emplace_back(m_scene->create(transform));
}

void TriangleScene::createCamera()
{
    auto camera = std::make_shared<rdr::Camera>();
    camera->lookAt(rdr::math::float3(0.0, 0.0, 0.5), rdr::math::float3(0.0, 0.0, -1.0));
    camera->perspective(90, 1, 0.01, 1000);
    m_cameras.emplace_back(m_scene->create(camera));
}

void TriangleScene::registerResource()
{
    auto e0 = m_entities[0];
    auto e1 = m_entities[1];
    auto e2 = m_entities[2];

    auto g0 = m_geometrys[0];
    auto g1 = m_geometrys[1];
    auto t0 = m_transforms[0];
    auto m0 = m_materials[0];
    auto m1 = m_materials[1];
    m_scene->create(e0, g0, m0, t0);
    m_scene->create(e1, g1, m1, t0);

    auto c0 = m_cameras[0];
}