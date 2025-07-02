#include <scene/triangleLoader.h>

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

TriangleLoader::TriangleLoader(SceneMgr* scene) :
    Loader(scene)
{
}

TriangleLoader::~TriangleLoader()
{
}

void TriangleLoader::loadScene(const std::string& fileName)
{
    createGeometry();
    createMaterial();
    createTransform();
    createComponent();
}

void TriangleLoader::createGeometry()
{
    auto index   = acre::createUintBuffer();
    index->data  = (void*)g_index;
    index->count = 6;
    auto indexID = m_scene->createVIndexBuffer(index);

    auto position   = acre::createFloat3Buffer();
    position->data  = (void*)g_position;
    position->count = 4;
    auto positionID = m_scene->createVPositionBuffer(position);

    auto uv   = acre::createFloat2Buffer();
    uv->data  = (void*)g_uv;
    uv->count = 4;
    auto uvID = m_scene->createVUVBuffer(uv);

    auto geometry      = acre::createGeometry();
    geometry->index    = indexID;
    geometry->position = positionID;
    geometry->uv       = uvID;
    m_scene->create(geometry);

    geometry           = acre::createGeometry();
    geometry->index    = indexID;
    geometry->position = positionID;
    geometry->uv       = uvID;
    m_scene->create(geometry);
}

void TriangleLoader::createMaterial()
{
    auto image     = acre::createImage();
    image->data    = (void*)g_image;
    image->width   = 2;
    image->height  = 2;
    image->format  = acre::Image::Format::RGBA8_UNORM;
    auto imageID   = m_scene->create(image);
    auto texture   = acre::createTexture();
    texture->image = imageID;
    auto tex1      = m_scene->create(texture);

    std::string imgPath     = SRC_DIR "/res/img/desert.png";
    int         imgWidth    = 0;
    int         imgHeight   = 0;
    int         imgChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imgData = stbi_load(imgPath.c_str(), &imgWidth, &imgHeight, &imgChannels, 0);

    image          = std::make_shared<acre::Image>();
    image->data    = (void*)imgData;
    image->width   = imgWidth;
    image->height  = imgHeight;
    image->format  = acre::Image::Format::RGBA8_UNORM;
    imageID        = m_scene->create(image);
    texture        = acre::createTexture();
    texture->image = imageID;
    auto tex2      = m_scene->create(texture);

    {
        acre::StandardModel standard;
        standard.baseColor      = {1.0f, 0.0f, 0.0f};
        standard.baseColorIndex = tex1;
        auto material           = std::make_shared<acre::Material>();
        material->type          = acre::MaterialModel::mStandard;
        m_scene->create(material);
    }

    {
        acre::StandardModel standard;
        standard.baseColor      = {0.0f, 1.0f, 0.0f};
        standard.baseColorIndex = tex2;
        auto material           = std::make_shared<acre::Material>();
        material->type          = acre::MaterialModel::mStandard;
        m_scene->create(material);
    }
}

void TriangleLoader::createTransform()
{
    auto transform    = acre::createTransform();
    transform->matrix = acre::math::float4x4::identity();
    m_scene->create(transform);
}

void TriangleLoader::createComponent()
{
    // Note: from createGeometry
    auto g0 = 0;
    auto g1 = 1;

    // Note: from createMaterial
    auto m0 = 0;
    auto m1 = 1;

    // Note: from createTransform
    auto t0 = 0;

    // create components
    auto c0 = acre::component::createDraw(m_scene->create(std::make_shared<acre::Entity>("0")),
                                          g0, m0, t0);
    auto c1 = acre::component::createDraw(m_scene->create(std::make_shared<acre::Entity>("1")),
                                          g1, m1, t0);

    m_scene->create(c0);
    m_scene->create(c1);
}