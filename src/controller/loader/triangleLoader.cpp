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

constexpr float g_color[4][4] = {
    {1.0, 0.0, 0.0, 1.0},
    {0.0, 1.0, 0.0, 1.0},
    {0.0, 0.0, 1.0, 1.0},
    {1.0, 1.0, 1.0, 1.0},
};

// Use inverse clockwise order
constexpr uint32_t g_index[] = {
    0,
    1,
    2, // left bottom, right bottom, right top
    0,
    2,
    3, // left bottom, right top, left bottom

    1,
    0,
    3, // left bottom, left bottom, right top
    1,
    3,
    2, // left bottom, right top, left top
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
    create_component_draw();
}

void TriangleLoader::createGeometry()
{
    auto indexR  = m_scene->create<acre::VIndexID>(0);
    auto index   = indexR->ptr<acre::VIndexID>();
    index->data  = (void*)g_index;
    index->count = sizeof(g_index) / sizeof(uint32_t);

    auto positionR  = m_scene->create<acre::VPositionID>(0);
    auto position   = positionR->ptr<acre::VPositionID>();
    position->data  = (void*)g_position;
    position->count = 4;

    auto uvR  = m_scene->create<acre::VUVID>(0);
    auto uv   = uvR->ptr<acre::VUVID>();
    uv->data  = (void*)g_uv;
    uv->count = 4;

    auto normalR  = m_scene->create<acre::VNormalID>(0);
    auto normal   = normalR->ptr<acre::VNormalID>();
    normal->data  = (void*)g_normal;
    normal->count = 4;

    auto colorR  = m_scene->create<acre::VColorID>(0);
    auto color   = colorR->ptr<acre::VColorID>();
    color->data  = (void*)g_color;
    color->count = 4;

    auto node     = m_scene->create<acre::GeometryID>(0);
    auto geo      = node->ptr<acre::GeometryID>();
    geo->index    = {index, indexR->idx()};
    geo->position = {position, positionR->idx()};
    geo->uv       = {uv, uvR->idx()};
    geo->normal   = {normal, normalR->idx()};
    geo->color    = {color, colorR->idx()};
}

void TriangleLoader::createMaterial()
{
    auto imageR   = m_scene->create<acre::ImageID>(0);
    auto image    = imageR->ptr<acre::ImageID>();
    image->data   = (void*)g_image;
    image->width  = 2;
    image->height = 2;
    image->format = acre::Image::Format::RGBA8_UNORM;

    auto textureR  = m_scene->create<acre::TextureID>(0);
    auto texture   = textureR->ptr<acre::TextureID>();
    texture->image = {image, imageR->idx()};

    acre::SimpleModel model;
    model.color     = {1.0f, 1.0f, 1.0f};
    model.color_idx = {texture, textureR->idx()};

    auto materialR  = m_scene->create<acre::MaterialID>(0);
    auto material   = materialR->ptr<acre::MaterialID>();
    material->type  = acre::MaterialModel::mSimple;
    material->model = model;
}

void TriangleLoader::createTransform()
{
    auto transformR   = m_scene->create<acre::TransformID>(0);
    auto transform    = transformR->ptr<acre::TransformID>();
    transform->matrix = acre::math::float4x4::identity();
    transform->affine = acre::math::affine3::identity();
}

void TriangleLoader::create_component_draw()
{
    // Note: from createGeometry
    auto geo = m_scene->find<acre::GeometryID>(0)->id<acre::GeometryID>();

    // Note: from createMaterial
    auto mat = m_scene->find<acre::MaterialID>(0)->id<acre::MaterialID>();

    // Note: from createTransform
    auto trans = m_scene->find<acre::TransformID>(0)->id<acre::TransformID>();

    auto entity = m_scene->create<acre::EntityID>(0);

    // create components
    auto draw       = acre::component::createDraw();
    draw->entity    = entity->id<acre::EntityID>();
    draw->geometry  = geo;
    draw->material  = mat;
    draw->transform = trans;
    m_scene->create(draw);

    auto sceneBox   = acre::math::box3::empty();
    sceneBox.m_maxs = {1, 1, 1};
    sceneBox.m_mins = {-1, -1, -1};

    m_scene->merge_box(sceneBox);
}
