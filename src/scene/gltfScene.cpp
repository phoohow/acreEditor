#include <scene/gltfScene.h>
#include <string>
#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>
#include <acre/render/renderer.h>

static constexpr double M_PI = 3.14159265358979;

template <typename T>
static auto vec3ToFloat3(T& vec)
{
    return acre::math::float3(vec[0], vec[1], vec[2]);
}

template <typename T>
static auto vec4ToFloat4(T& vec)
{
    return acre::math::float4(vec[0], vec[1], vec[2], vec[3]);
}

template <typename T>
static auto vec16ToFloat4x4(T& vec)
{
    return acre::math::float4x4(vec[0], vec[1], vec[2], vec[3],
                                vec[4], vec[5], vec[6], vec[7],
                                vec[8], vec[9], vec[10], vec[11],
                                vec[12], vec[13], vec[14], vec[15]);
}

template <typename T>
static auto vec4ToQuat(T& vec)
{
    return acre::math::quat(vec[3], vec[0], vec[1], vec[2]);
}

static auto toImageFormat(int component, int bits)
{
    if (component == 3)
    {
        switch (bits)
        {
            case 8:
                return acre::ImageFormat::RGB8;
            case 16:
                return acre::ImageFormat::RGB16;
            case 32:
                return acre::ImageFormat::RGB32;
            default:
                printf("Unsupport type!\n");
                return acre::ImageFormat::RGB32;
        }
    }
    else if (component == 4)
    {
        switch (bits)
        {
            case 8:
                return acre::ImageFormat::RGBA8;
            case 16:
                return acre::ImageFormat::RGBA16;
            case 32:
                return acre::ImageFormat::RGBA32;
            default:
                printf("Unsupport type!\n");
                return acre::ImageFormat::RGBA32;
        }
    }

    return acre::ImageFormat::RGBA32;
}

template <typename Camera, typename Box, typename Direction>
static void setCameraView(Camera camera, Box box, Direction dir)
{
    auto fov      = 45.0f;
    auto radius   = acre::math::length(box.diagonal()) * 0.5f;
    auto distance = radius / sinf(acre::math::radians(fov * 0.5f));
    camera->lookAt(box.center() + dir * distance, box.center());
    camera->resetYaw();
    camera->resetPitch();
}

using namespace tinygltf;
std::vector<std::vector<uint32_t>> g_index;
std::map<std::string, int>         g_geometry;

template <typename Type>
static void appendIndex(acre::GeometryPtr           geometry,
                        const tinygltf::Accessor&   accessor,
                        const tinygltf::BufferView& bufferView,
                        uint32_t                    index,
                        unsigned char*              addr)
{
    std::vector<Type> tempVec;
    g_index[index].resize(accessor.count);
    tempVec.resize(accessor.count);
    memcpy(tempVec.data(), (void*)(addr + bufferView.byteOffset + accessor.byteOffset), sizeof(Type) * accessor.count);
    for (auto i = 0; i < accessor.count; ++i)
        g_index[index][i] = tempVec[i];

    geometry->indexData = g_index[index].data();
}

GLTFScene::GLTFScene(acre::Scene* scene) :
    BaseScene(scene)
{
    m_model  = new tinygltf::Model;
    m_loader = new tinygltf::TinyGLTF;
}

GLTFScene::~GLTFScene()
{
    delete m_model;
    delete m_loader;
}

void GLTFScene::createScene()
{
    m_box = acre::math::box3::empty();
    clearScene();

    createMaterial();
    createGeometry();
    createTransform();

    registerResource();
    setCamera();
}

void GLTFScene::loadGLTF(const std::string& fileName)
{
    std::string warn;
    std::string err;

    size_t      dotIndex  = fileName.find_last_of('.');
    std::string extension = fileName.substr(dotIndex + 1);

    bool ret = false;
    if (extension == "gltf")
    {
        ret = m_loader->LoadASCIIFromFile(m_model, &err, &warn, fileName.c_str());
    }
    else if (extension == "glb")
    {
        ret = m_loader->LoadBinaryFromFile(m_model, &err, &warn, fileName.c_str());
    }
    else
    {
        printf("Unsupported file format");
        return;
    }

    if (!warn.empty())
    {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty())
    {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret)
    {
        printf("Failed to parse glTF\n");
    }
}

void GLTFScene::loadHDR(const std::string& fileName)
{
    clearHDR();

    int  width;
    int  height;
    int  channels;
    int  desired = 4;
    auto hdrData = stbi_loadf(fileName.c_str(), &width, &height, &channels, desired);

    auto image     = std::make_shared<acre::Image>();
    image->name    = "hdrTexture";
    image->data    = (hdrData);
    image->width   = width;
    image->height  = height;
    image->format  = desired == 3 ? acre::ImageFormat::RGB32 : acre::ImageFormat::RGBA32;
    image->mipmaps = log2(width >= height ? width : height);
    auto imageID   = m_scene->create(image);
    m_imageExts.emplace_back(imageID);

    auto texture   = std::make_shared<acre::Texture>();
    texture->image = imageID;
    auto textureID = m_scene->create(texture);
    m_textureExts.emplace_back(textureID);

    auto light = std::make_shared<acre::HDRLight>();
    light->id  = textureID;
    m_scene->setHDRLight(light);
    m_scene->enableHDR();
}

void GLTFScene::saveFrame(const std::string& fileName, acre::Pixels* pixels)
{
    stbi_write_bmp(fileName.c_str(), pixels->width, pixels->height, 4, (void*)pixels->data);
}

void GLTFScene::createMaterial()
{
    for (const auto& img : m_model->images)
    {
        auto image    = acre::createImage();
        image->data   = (void*)img.image.data();
        image->width  = img.width;
        image->height = img.height;
        image->name   = img.name.c_str();
        image->format = toImageFormat(img.component, img.bits);
        auto imageID  = m_scene->create(image);
        m_images.emplace_back(imageID);
    }

    for (auto& tex : m_model->textures)
    {
        auto texture   = acre::createTexture();
        texture->image = m_images[tex.source];
        auto textureID = m_scene->create(texture);
        m_textures.emplace_back(textureID);
    }

    for (const auto& mat : m_model->materials)
    {
        auto material = acre::createMaterial();
        // Common
        material->normalIndex   = mat.normalTexture.index != -1 ? m_textures[mat.normalTexture.index] : -1;
        material->emission      = vec3ToFloat3(mat.emissiveFactor);
        material->emissionIndex = mat.emissiveTexture.index != -1 ? m_textures[mat.emissiveTexture.index] : -1;

        const auto& exts = mat.extensions;
        if (exts.find("KHR_materials_anisotropy") != exts.end())
        {
            material->surfaceModel = acre::MaterialSurfaceModel::Anisotropy;
            acre::SurfaceAnisotropy surface;
            auto                    baseColor = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
            surface.baseColor                 = baseColor.xyz();
            material->alpha                   = baseColor.w;
            surface.baseColorIndex            = mat.pbrMetallicRoughness.baseColorTexture.index != -1 ?
                           m_textures[mat.pbrMetallicRoughness.baseColorTexture.index] :
                           -1;
            surface.roughness                 = mat.pbrMetallicRoughness.roughnessFactor;
            surface.metallic                  = mat.pbrMetallicRoughness.metallicFactor;
            surface.metalRoughIndex           = mat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1 ?
                          m_textures[mat.pbrMetallicRoughness.metallicRoughnessTexture.index] :
                          -1;

            const auto& anisotropy         = exts.find("KHR_materials_anisotropy");
            const auto& anisotropyStrength = anisotropy->second.Get("anisotropyStrength");
            surface.anisotropy             = anisotropyStrength.GetNumberAsDouble();

            if (anisotropy->second.Has("anisotropyRotation"))
            {
                const auto& anisotropyRotation = anisotropy->second.Get("anisotropyRotation");
                surface.anisotropyRotation     = anisotropyRotation.GetNumberAsDouble();
            }
            if (anisotropy->second.Has("anisotropyTexture"))
            {
                const auto& anisotropyTexture = anisotropy->second.Get("anisotropyTexture");
                surface.anisotropyIndex       = anisotropyTexture.GetNumberAsInt();
            }

            material->surface = surface;
        }
        else if (exts.find("KHR_materials_iridescence") != exts.end())
        {
            material->surfaceModel = acre::MaterialSurfaceModel::Iridescence;
            acre::SurfaceIrridescence surface;
            auto                      baseColor = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
            surface.baseColor                   = baseColor.xyz();
            material->alpha                     = baseColor.w;
            surface.baseColorIndex              = mat.pbrMetallicRoughness.baseColorTexture.index != -1 ?
                             m_textures[mat.pbrMetallicRoughness.baseColorTexture.index] :
                             -1;
            surface.roughness                   = mat.pbrMetallicRoughness.roughnessFactor;
            surface.metallic                    = mat.pbrMetallicRoughness.metallicFactor;
            surface.metalRoughIndex             = mat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1 ?
                            m_textures[mat.pbrMetallicRoughness.metallicRoughnessTexture.index] :
                            -1;

            const auto& iridescence       = exts.find("KHR_materials_iridescence");
            const auto& iridescenceFactor = iridescence->second.Get("iridescenceFactor");
            surface.iridescence           = iridescenceFactor.GetNumberAsDouble();
            if (iridescence->second.Has("iridescenceTexture"))
            {
                const auto& iridescenceTexture = iridescence->second.Get("iridescenceTexture");
                surface.iridescenceIndex       = iridescenceTexture.GetNumberAsInt();
            }
            if (iridescence->second.Has("iridescenceIor"))
            {
                const auto& iridescenceIor = iridescence->second.Get("iridescenceIor");
                surface.iridescenceIor     = iridescenceIor.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessMaximum"))
            {
                const auto& iridescenceThicknessMaximum = iridescence->second.Get("iridescenceThicknessMaximum");
                surface.iridescenceThicknessMax         = iridescenceThicknessMaximum.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessMinimum"))
            {
                const auto& iridescenceThicknessMinimum = iridescence->second.Get("iridescenceThicknessMinimum");
                surface.iridescenceThicknessMin         = iridescenceThicknessMinimum.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessTexture"))
            {
                const auto& iridescenceThicknessTexture = iridescence->second.Get("iridescenceThicknessTexture");
                surface.iridescenceThicknessIndex       = iridescenceThicknessTexture.GetNumberAsInt();
            }

            material->surface = surface;
        }
        else
        {
            material->surfaceModel = acre::MaterialSurfaceModel::Standard;
            acre::SurfaceStandard surface;
            auto                  baseColor = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
            surface.baseColor               = baseColor.xyz();
            material->alpha                 = baseColor.w;
            surface.baseColorIndex          = mat.pbrMetallicRoughness.baseColorTexture.index != -1 ?
                         m_textures[mat.pbrMetallicRoughness.baseColorTexture.index] :
                         -1;
            surface.roughness               = mat.pbrMetallicRoughness.roughnessFactor;
            surface.metallic                = mat.pbrMetallicRoughness.metallicFactor;
            surface.metalRoughIndex         = mat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1 ?
                        m_textures[mat.pbrMetallicRoughness.metallicRoughnessTexture.index] :
                        -1;

            material->surface = surface;
        }

        const auto& transmission = exts.find("KHR_materials_transmission");
        if (transmission != exts.end())
        {
            const auto& transmissionFactor = transmission->second.Get("transmissionFactor");
            const auto& value              = transmissionFactor.GetNumberAsDouble();
            material->alpha                = value;
            // TODO: test code, complete it later
            material->alphaIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
        }

        if (mat.alphaMode == "OPAQUE" || mat.alphaMode == "MASK")
        {
            material->alpha = 1.0f;
        }

        auto materialID = m_scene->create(material);
        m_materials.emplace_back(materialID);
    }
}

void GLTFScene::createGeometry()
{
    g_geometry.clear();

    int geometryIndex = 0;
    for (int meshIndex = 0; meshIndex < m_model->meshes.size(); ++meshIndex)
    {
        const auto& mesh = m_model->meshes[meshIndex];
        g_index.resize(g_index.size() + mesh.primitives.size());

        for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
        {
            const auto& primitive = mesh.primitives[primitiveIndex];
            auto        geometry  = acre::createGeometry();

            if (primitive.indices > -1)
            {
                const auto& accessor   = m_model->accessors[primitive.indices];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                switch (accessor.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                        appendIndex<int8_t>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        appendIndex<uint8_t>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                        appendIndex<int16_t>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        appendIndex<uint16_t>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_INT:
                        appendIndex<int32_t>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        appendIndex<uint32_t>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        appendIndex<float>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                        appendIndex<double>(geometry, accessor, bufferView, geometryIndex, addr);
                        break;
                    default:
                        geometry->indexData = addr + bufferView.byteOffset + accessor.byteOffset;
                        break;
                }

                geometry->indexCount = accessor.count;
            }
            if (primitive.attributes.find("POSITION") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("POSITION")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("Undo!\n");
                }
                geometry->positionData = addr + bufferView.byteOffset + accessor.byteOffset;
                geometry->vertexCount  = accessor.count;

                // Evaluate object box and scene box
                acre::math::box3 box = acre::math::box3::empty();
                for (auto geometryIndex = 0; geometryIndex < geometry->vertexCount; geometryIndex += 3)
                {
                    acre::math::float3* pos = (acre::math::float3*)(geometry->positionData) + geometryIndex;
                    box |= *pos;
                }
                geometry->box = box;
            }
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("TEXCOORD_0")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("Undo!\n");
                }
                geometry->uvData = addr + bufferView.byteOffset + accessor.byteOffset;
            }
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("NORMAL")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("Undo!\n");
                }
                geometry->normalData = addr + bufferView.byteOffset + accessor.byteOffset;
            }
            if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("TANGENT")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("Undo!\n");
                }
                geometry->tangentData = addr + bufferView.byteOffset + accessor.byteOffset;
            }

            auto geometryID = m_scene->create(geometry);
            m_geometrys.emplace_back(geometryID);

            auto key = std::to_string(meshIndex) + "_" + std::to_string(primitiveIndex);
            g_geometry.emplace(key, geometryIndex++);
        }
    }
}

void GLTFScene::createTransform()
{
    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        auto node = m_model->nodes[nodeIndex];

        auto affine = acre::math::affine3::identity();
        if (!node.scale.empty())
        {
            affine *= acre::math::scaling(vec3ToFloat3(node.scale));
        }
        if (!node.rotation.empty())
        {
            auto rotate = vec4ToQuat(node.rotation);
            affine *= rotate.toAffine();
        }
        if (!node.translation.empty())
        {
            affine *= acre::math::translation(vec3ToFloat3(node.translation));
        }

        auto transform    = acre::createTransform();
        transform->affine = affine;
        transform->matrix = acre::math::affineToHomogeneous(affine);
        m_transforms.emplace_back(m_scene->create(transform));
    }

    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        auto node      = m_model->nodes[nodeIndex];
        auto transform = m_scene->findTransform(nodeIndex);
        for (auto childIndex : node.children)
        {
            auto childTransform = m_scene->findTransform(childIndex);
            childTransform->matrix *= transform->matrix;
            childTransform->affine *= transform->affine;
        }
    }
}

void GLTFScene::setCamera()
{
    auto camera = m_scene->getMainCamera();
    auto fov    = 45.0f;
    auto radius = acre::math::length(m_box.diagonal()) * 0.5f;
    camera->perspective(fov, float(m_width) / float(m_height), radius * 0.1, 100000.0f);

    forwardView();
}

void GLTFScene::registerResource()
{
    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        const auto& node = m_model->nodes[nodeIndex];
        if (node.mesh == -1)
            continue;
        const auto& transformID = nodeIndex;
        const auto& transform   = m_scene->findTransform(transformID);
        const auto& mesh        = m_model->meshes[node.mesh];
        for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
        {
            auto key = std::to_string(node.mesh) + "_" + std::to_string(primitiveIndex);

            auto entity   = acre::createEntity(key);
            auto entityID = m_scene->create(entity);
            m_entities.emplace_back(entityID);

            auto  geometryIndex = g_geometry[key];
            auto  geometryID    = m_geometrys[geometryIndex];
            auto  geometry      = m_scene->findGeometry(geometryID);
            auto& box           = geometry->box;
            box                 = box * transform->affine;
            m_box |= box;

            const auto& primitive  = mesh.primitives[primitiveIndex];
            auto        materialID = m_materials[primitive.material];

            m_scene->createComponent(acre::createComponentDraw(entityID, geometryID, materialID, transformID));
        }
    }
}

void GLTFScene::resize(uint32_t width, uint32_t height)
{
    m_width  = width;
    m_height = height;

    auto fov        = 45.0f;
    auto mainCamera = m_scene->getMainCamera();
    if (!mainCamera)
        return;

    auto radius = acre::math::length(m_box.diagonal()) * 0.5f;
    mainCamera->perspective(fov, float(width) / float(height), radius * 0.1, 100000.0f);
}

void GLTFScene::cameraMove(acre::math::float3 delta)
{
    auto mainCamera = m_scene->getMainCamera();
    auto radius     = acre::math::length(m_box.diagonal()) * 0.5f;
    mainCamera->translate(delta * radius * 0.02f);
}

void GLTFScene::cameraForward()
{
    auto mainCamera = m_scene->getMainCamera();
    auto dir        = mainCamera->getDirection();
    cameraMove(dir);
}

void GLTFScene::cameraBack()
{
    auto mainCamera = m_scene->getMainCamera();
    auto dir        = mainCamera->getDirection();
    cameraMove(-dir);
}

void GLTFScene::cameraRotateY(float degree)
{
    auto mainCamera = m_scene->getMainCamera();
    mainCamera->rotate(0, acre::math::radians(degree));
}

void GLTFScene::cameraRotateX(float degree)
{
    auto mainCamera = m_scene->getMainCamera();
    mainCamera->rotate(acre::math::radians(degree), 0);
}

void GLTFScene::leftView()
{
    setCameraView(m_scene->getMainCamera(), m_box, acre::math::float3(1, 0, 0));
    m_scene->getMainCamera()->rotate(0, M_PI * 1.5);
}

void GLTFScene::rightView()
{
    setCameraView(m_scene->getMainCamera(), m_box, acre::math::float3(-1, 0, 0));
    m_scene->getMainCamera()->rotate(0, M_PI * 0.5);
}

void GLTFScene::forwardView()
{
    setCameraView(m_scene->getMainCamera(), m_box, acre::math::float3(0, 0, 1));
    m_scene->getMainCamera()->rotate(0, M_PI);
}

void GLTFScene::backView()
{
    setCameraView(m_scene->getMainCamera(), m_box, acre::math::float3(0, 0, -1));
    m_scene->getMainCamera()->rotate(0, 0);
}

void GLTFScene::topView()
{
    setCameraView(m_scene->getMainCamera(), m_box, acre::math::float3(0, 1, 0));
    m_scene->getMainCamera()->rotate(M_PI * 0.5, 0);
}

void GLTFScene::bottomView()
{
    setCameraView(m_scene->getMainCamera(), m_box, acre::math::float3(0, -1, 0));
    m_scene->getMainCamera()->rotate(-M_PI * 0.5, 0);
}
