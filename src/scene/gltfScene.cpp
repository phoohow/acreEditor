#include <scene/gltfScene.h>
#include <string>
#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>
#include <acre/render/renderer.h>

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
            // case 8:
            //     return acre::Image::Format::RGB8;
            // case 16:
            //     return acre::Image::Format::RGB16;
            case 32:
                return acre::Image::Format::RGB32_FLOAT;
            default:
                printf("Unsupport type!\n");
                return acre::Image::Format::RGB32_FLOAT;
        }
    }
    else if (component == 4)
    {
        switch (bits)
        {
            case 8:
                return acre::Image::Format::RGBA8_UNORM;
            case 16:
                return acre::Image::Format::RGBA16_FLOAT;
            case 32:
                return acre::Image::Format::RGBA32_FLOAT;
            default:
                printf("Unsupport type!\n");
                return acre::Image::Format::RGBA32_FLOAT;
        }
    }

    return acre::Image::Format::RGBA32_FLOAT;
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
    setMainCamera();
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
    int  width;
    int  height;
    int  channels;
    int  desired = 4;
    auto hdrData = stbi_loadf(fileName.c_str(), &width, &height, &channels, desired);

    auto image     = acre::createImage();
    image->name    = "hdrTexture";
    image->data    = (hdrData);
    image->width   = width;
    image->height  = height;
    image->format  = desired == 3 ? acre::Image::Format::RGB32_FLOAT : acre::Image::Format::RGBA32_FLOAT;
    image->mipmaps = log2(width >= height ? width : height);
    auto imageID   = m_scene->create(image);

    auto texture   = acre::createTexture();
    texture->image = imageID;
    auto textureID = m_scene->create(texture);

    auto light    = std::make_shared<acre::HDRLight>();
    light->id     = textureID;
    light->enable = true;
    m_scene->setHDRLight(light);

    clearHDR();
    m_imageExts.emplace_back(imageID);
    m_textureExts.emplace_back(textureID);
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
        auto material  = acre::createMaterial();
        material->type = acre::MaterialModel::mStandard;
        acre::StandardModel model;
        auto                baseColor = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
        model.baseColor               = baseColor.xyz();
        material->alpha               = baseColor.w;
        model.baseColorIndex          = mat.pbrMetallicRoughness.baseColorTexture.index != -1 ?
                     m_textures[mat.pbrMetallicRoughness.baseColorTexture.index] :
                     -1;
        model.roughness               = mat.pbrMetallicRoughness.roughnessFactor;
        model.metallic                = mat.pbrMetallicRoughness.metallicFactor;
        model.metalRoughIndex         = mat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1 ?
                    m_textures[mat.pbrMetallicRoughness.metallicRoughnessTexture.index] :
                    -1;
        model.normalIndex             = mat.normalTexture.index != -1 ? m_textures[mat.normalTexture.index] : -1;
        model.emission                = vec3ToFloat3(mat.emissiveFactor);
        model.emissionIndex           = mat.emissiveTexture.index != -1 ? m_textures[mat.emissiveTexture.index] : -1;

        const auto& exts = mat.extensions;
        if (exts.find("KHR_materials_ior") != exts.end())
        {
            const auto& ior = exts.find("KHR_materials_ior");
            model.ior       = ior->second.Get("ior").GetNumberAsDouble();
        }

        if (exts.find("KHR_materials_clearcoat") != exts.end())
        {
            model.useClearcoat    = true;
            const auto& clearcoat = exts.find("KHR_materials_clearcoat");

            const auto& clearcoatFactor          = clearcoat->second.Get("clearcoatFactor");
            model.clearcoat                      = clearcoatFactor.GetNumberAsDouble();
            const auto& clearcoatRoughnessFactor = clearcoat->second.Get("clearcoatRoughnessFactor");
            model.clearcoatRoughness             = clearcoatRoughnessFactor.GetNumberAsDouble();
            if (clearcoat->second.Has("clearcoatTexture"))
            {
                const auto& clearcoatTexture = clearcoat->second.Get("clearcoatTexture");
                model.clearcoatIndex         = m_textures[clearcoatTexture.Get("index").GetNumberAsInt()];
            }
            if (clearcoat->second.Has("clearcoatRoughnessTexture"))
            {
                const auto& clearcoatRoughnessTexture = clearcoat->second.Get("clearcoatRoughnessTexture");
                model.clearcoatRoughnessIndex         = m_textures[clearcoatRoughnessTexture.Get("index").GetNumberAsInt()];
            }
            if (clearcoat->second.Has("clearcoatNormalTexture"))
            {
                const auto& clearcoatNormalTexture = clearcoat->second.Get("clearcoatNormalTexture");
                model.clearcoatNormalIndex         = m_textures[clearcoatNormalTexture.Get("index").GetNumberAsInt()];
            }
        }

        if (exts.find("KHR_materials_sheen") != exts.end())
        {
            model.useSheen    = true;
            const auto& sheen = exts.find("KHR_materials_sheen");

            const auto& sheenColorFactor     = sheen->second.Get("sheenColorFactor");
            model.sheenColor                 = acre::math::float3(sheenColorFactor.Get(0).GetNumberAsDouble(),
                                                                  sheenColorFactor.Get(1).GetNumberAsDouble(),
                                                                  sheenColorFactor.Get(2).GetNumberAsDouble());
            const auto& sheenRoughnessFactor = sheen->second.Get("sheenRoughnessFactor");
            model.sheenRoughness             = sheenRoughnessFactor.GetNumberAsDouble();
            if (sheen->second.Has("sheenColorTexture"))
            {
                const auto& sheenColorTexture = sheen->second.Get("sheenColorTexture");
                model.sheenColorIndex         = m_textures[sheenColorTexture.Get("index").GetNumberAsInt()];
            }
            if (sheen->second.Has("sheenRoughnessTexture"))
            {
                const auto& sheenRoughnessTexture = sheen->second.Get("sheenRoughnessTexture");
                model.sheenRoughnessIndex         = m_textures[sheenRoughnessTexture.Get("index").GetNumberAsInt()];
            }
        }

        if (exts.find("KHR_materials_anisotropy") != exts.end())
        {
            model.useAnisotropy    = true;
            const auto& anisotropy = exts.find("KHR_materials_anisotropy");

            const auto& anisotropyStrength = anisotropy->second.Get("anisotropyStrength");
            model.anisotropy               = anisotropyStrength.GetNumberAsDouble();

            if (anisotropy->second.Has("anisotropyRotation"))
            {
                const auto& anisotropyRotation = anisotropy->second.Get("anisotropyRotation");
                model.anisotropyRotation       = anisotropyRotation.GetNumberAsDouble();
            }
            if (anisotropy->second.Has("anisotropyTexture"))
            {
                const auto& anisotropyTexture = anisotropy->second.Get("anisotropyTexture");
                model.anisotropyIndex         = m_textures[anisotropyTexture.Get("index").GetNumberAsInt()];
            }
        }

        if (exts.find("KHR_materials_iridescence") != exts.end())
        {
            model.useIridescence    = true;
            const auto& iridescence = exts.find("KHR_materials_iridescence");

            const auto& iridescenceFactor = iridescence->second.Get("iridescenceFactor");
            model.iridescence             = iridescenceFactor.GetNumberAsDouble();
            if (iridescence->second.Has("iridescenceTexture"))
            {
                const auto& iridescenceTexture = iridescence->second.Get("iridescenceTexture");
                model.iridescenceIndex         = m_textures[iridescenceTexture.Get("index").GetNumberAsInt()];
            }
            if (iridescence->second.Has("iridescenceIor"))
            {
                const auto& iridescenceIor = iridescence->second.Get("iridescenceIor");
                model.iridescenceIor       = iridescenceIor.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessMaximum"))
            {
                const auto& iridescenceThicknessMaximum = iridescence->second.Get("iridescenceThicknessMaximum");
                model.iridescenceThicknessMax           = iridescenceThicknessMaximum.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessMinimum"))
            {
                const auto& iridescenceThicknessMinimum = iridescence->second.Get("iridescenceThicknessMinimum");
                model.iridescenceThicknessMin           = iridescenceThicknessMinimum.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessTexture"))
            {
                const auto& iridescenceThicknessTexture = iridescence->second.Get("iridescenceThicknessTexture");
                model.iridescenceThicknessIndex         = m_textures[iridescenceThicknessTexture.Get("index").GetNumberAsInt()];
            }
        }

        const auto& transmission = exts.find("KHR_materials_transmission");
        if (transmission != exts.end())
        {
            model.useTransmission          = true;
            const auto& transmissionFactor = transmission->second.Get("transmissionFactor");

            const auto& value = transmissionFactor.GetNumberAsDouble();
            material->alpha   = value;
            // TODO: test code, complete it later
            material->alphaIndex = mat.pbrMetallicRoughness.baseColorTexture.index != -1 ?
                m_textures[mat.pbrMetallicRoughness.baseColorTexture.index] :
                -1;

            model.transmission = transmissionFactor.GetNumberAsDouble();
            if (transmission->second.Has("transmissionTexture"))
            {
                const auto& transmissionTexture = transmission->second.Get("transmissionTexture");
                model.transmissionIndex         = m_textures[transmissionTexture.Get("index").GetNumberAsInt()];
            }

            const auto& volumeExt = exts.find("KHR_materials_volume");
            if (volumeExt != exts.end())
            {
                const auto& thicknessFactor = volumeExt->second.Get("thicknessFactor");
                model.thickness             = thicknessFactor.GetNumberAsDouble();

                if (volumeExt->second.Has("thicknessTexture"))
                {
                    const auto& thicknessTexture = volumeExt->second.Get("thicknessTexture");
                    model.thicknessIndex         = m_textures[thicknessTexture.Get("index").GetNumberAsInt()];
                }

                if (volumeExt->second.Has("attenuationDistance"))
                {
                    const auto& attenuationDistance = volumeExt->second.Get("attenuationDistance");
                    model.attenuationDistance       = attenuationDistance.GetNumberAsDouble();
                }

                if (volumeExt->second.Has("attenuationColor"))
                {
                    const auto& attenuationColor = volumeExt->second.Get("attenuationColor");
                    model.attenuationColor       = acre::math::float3(attenuationColor.Get(0).GetNumberAsDouble(),
                                                                      attenuationColor.Get(1).GetNumberAsDouble(),
                                                                      attenuationColor.Get(2).GetNumberAsDouble());
                }
            }
        }

        if (mat.alphaMode == "OPAQUE" || mat.alphaMode == "MASK")
        {
            material->alpha = 1.0f;
        }

        material->model = model;
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

        if (!node.matrix.empty())
        {
            auto transform    = acre::createTransform();
            transform->matrix = vec16ToFloat4x4(node.matrix);
            transform->affine = acre::math::homogeneousToAffine(transform->matrix);
            m_transforms.emplace_back(m_scene->create(transform));
        }
        else
        {
            auto transform    = acre::createTransform();
            transform->affine = affine;
            transform->matrix = acre::math::affineToHomogeneous(affine);
            m_transforms.emplace_back(m_scene->create(transform));
        }
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

            m_scene->createComponentDraw(acre::component::createDraw(entityID, geometryID, materialID, transformID));
        }
    }
}
