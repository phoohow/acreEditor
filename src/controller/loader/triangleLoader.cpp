#include <config.h>

#include <controller/loader/triangleLoader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>

constexpr float g_position[4][3] = {
    {-1.0, -1.0, 0.5}, // left bottom
    {1.0, -1.0, 0.5},  // right bottom
    {1.0, 1.0, 0.5},   // right top
    {-1.0, 1.0, 0.5},  // left top
};

constexpr float g_uv[4][2] = {
    {0.0, 0.0}, // left bottom
    {1.0, 0.0}, // right bottom
    {1.0, 1.0}, // right top
    {0.0, 1.0}, // left top
};

constexpr float g_normal[4][3] = {
    {0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0},
};

// Use inverse clockwise order
constexpr uint32_t g_index[] = {
    0, 1, 2, // left bottom, right bottom, right top
    0, 2, 3, // left bottom, right top, left bottom

    1, 0, 3, // left bottom, left bottom, right top
    1, 3, 2, // left bottom, right top, left top
};

uint8_t g_image[][4] = {
    {255, 0, 0, 255},
    {0, 255, 0, 255},
    {0, 0, 255, 255},
    {255, 255, 255, 255},
};

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
    index->count = sizeof(g_index) / sizeof(uint32_t);

    auto position   = acre::createFloat3Buffer();
    position->data  = (void*)g_position;
    position->count = 4;

    auto uv   = acre::createFloat2Buffer();
    uv->data  = (void*)g_uv;
    uv->count = 4;

    auto normal   = acre::createFloat3Buffer();
    normal->data  = (void*)g_normal;
    normal->count = 4;

    auto geo      = acre::createGeometry();
    geo->index    = m_scene->createVIndexBuffer(index);
    geo->position = m_scene->createVPositionBuffer(position);
    geo->uv       = m_scene->createVUVBuffer(uv);
    geo->normal   = m_scene->createVNormalBuffer(normal);
    m_scene->create(geo);
}

void TriangleLoader::createMaterial()
{
    auto image    = acre::createImage();
    image->data   = (void*)g_image;
    image->width  = 2;
    image->height = 2;
    image->format = acre::Image::Format::RGBA8_UNORM;

    auto texture   = acre::createTexture();
    texture->image = m_scene->create(image);

    acre::StandardModel standard;
    standard.baseColor      = {1.0f, 1.0f, 1.0f};
    standard.baseColorIndex = m_scene->create(texture);
    auto material           = acre::createMaterial();
    material->type          = acre::MaterialModel::mStandard;
    material->model         = standard;
    m_scene->create(material);
}

void TriangleLoader::createTransform()
{
    auto transform    = acre::createTransform();
    transform->matrix = acre::math::float4x4::identity();
    transform->affine = acre::math::affine3::identity();
    m_scene->create(transform);
}

void TriangleLoader::createComponent()
{
    // Note: from createGeometry
    auto geo = 0;

    // Note: from createMaterial
    auto mat = 0;

    // Note: from createTransform
    auto trans = 0;

    // create components
    auto draw = acre::component::createDraw(m_scene->create(std::make_shared<acre::Entity>("0")),
                                            geo, mat, trans);

    m_scene->create(draw);

    auto sceneBox   = acre::math::box3::empty();
    sceneBox.m_maxs = {1, 1, 1};
    sceneBox.m_mins = {-1, -1, -1};

    m_scene->mergeBox(sceneBox);
}