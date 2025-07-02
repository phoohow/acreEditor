#include <controller/loader/gltfLoader.h>

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
static void appendIndex(acre::UintBufferPtr         indexBuffer,
                        const tinygltf::Accessor&   accessor,
                        const tinygltf::BufferView& bufferView,
                        uint32_t                    index,
                        unsigned char*              addr)
{
    std::vector<Type> tempVec;
    tempVec.resize(accessor.count);
    memcpy(tempVec.data(), (void*)(addr + bufferView.byteOffset + accessor.byteOffset), sizeof(Type) * accessor.count);

    g_index[index].resize(accessor.count);
    for (auto i = 0; i < accessor.count; ++i)
        g_index[index][i] = (uint32_t)(tempVec[i]);

    indexBuffer->data = g_index[index].data();
}

GLTFLoader::GLTFLoader(SceneMgr* scene) :
    Loader(scene)
{
    m_model  = new tinygltf::Model;
    m_loader = new tinygltf::TinyGLTF;
}

GLTFLoader::~GLTFLoader()
{
    delete m_model;
    delete m_loader;
}

void GLTFLoader::loadScene(const std::string& fileName)
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

    createMaterial();
    createGeometry();
    createTransform();
    createComponent();
}

void GLTFLoader::createMaterial()
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
    }

    for (auto& tex : m_model->textures)
    {
        auto texture   = acre::createTexture();
        texture->image = m_scene->findImage(tex.source);
        auto textureID = m_scene->create(texture);
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
                     m_scene->findTexture(mat.pbrMetallicRoughness.baseColorTexture.index) :
                     -1;
        model.roughness               = mat.pbrMetallicRoughness.roughnessFactor;
        model.metallic                = mat.pbrMetallicRoughness.metallicFactor;
        model.metalRoughIndex         = mat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1 ?
                    m_scene->findTexture(mat.pbrMetallicRoughness.metallicRoughnessTexture.index) :
                    -1;
        model.normalIndex             = mat.normalTexture.index != -1 ? m_scene->findTexture(mat.normalTexture.index) : -1;
        model.emission                = vec3ToFloat3(mat.emissiveFactor);
        model.emissionIndex           = mat.emissiveTexture.index != -1 ? m_scene->findTexture(mat.emissiveTexture.index) : -1;

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
                model.clearcoatIndex         = m_scene->findTexture(clearcoatTexture.Get("index").GetNumberAsInt());
            }
            if (clearcoat->second.Has("clearcoatRoughnessTexture"))
            {
                const auto& clearcoatRoughnessTexture = clearcoat->second.Get("clearcoatRoughnessTexture");
                model.clearcoatRoughnessIndex         = m_scene->findTexture(clearcoatRoughnessTexture.Get("index").GetNumberAsInt());
            }
            if (clearcoat->second.Has("clearcoatNormalTexture"))
            {
                const auto& clearcoatNormalTexture = clearcoat->second.Get("clearcoatNormalTexture");
                model.clearcoatNormalIndex         = m_scene->findTexture(clearcoatNormalTexture.Get("index").GetNumberAsInt());
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
                model.sheenColorIndex         = m_scene->findTexture(sheenColorTexture.Get("index").GetNumberAsInt());
            }
            if (sheen->second.Has("sheenRoughnessTexture"))
            {
                const auto& sheenRoughnessTexture = sheen->second.Get("sheenRoughnessTexture");
                model.sheenRoughnessIndex         = m_scene->findTexture(sheenRoughnessTexture.Get("index").GetNumberAsInt());
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
                model.anisotropyIndex         = m_scene->findTexture(anisotropyTexture.Get("index").GetNumberAsInt());
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
                model.iridescenceIndex         = m_scene->findTexture(iridescenceTexture.Get("index").GetNumberAsInt());
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
                model.iridescenceThicknessIndex         = m_scene->findTexture(iridescenceThicknessTexture.Get("index").GetNumberAsInt());
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
                m_scene->findTexture(mat.pbrMetallicRoughness.baseColorTexture.index) :
                -1;

            model.transmission = transmissionFactor.GetNumberAsDouble();
            if (transmission->second.Has("transmissionTexture"))
            {
                const auto& transmissionTexture = transmission->second.Get("transmissionTexture");
                model.transmissionIndex         = m_scene->findTexture(transmissionTexture.Get("index").GetNumberAsInt());
            }

            const auto& volumeExt = exts.find("KHR_materials_volume");
            if (volumeExt != exts.end())
            {
                const auto& thicknessFactor = volumeExt->second.Get("thicknessFactor");
                model.thickness             = thicknessFactor.GetNumberAsDouble();

                if (volumeExt->second.Has("thicknessTexture"))
                {
                    const auto& thicknessTexture = volumeExt->second.Get("thicknessTexture");
                    model.thicknessIndex         = m_scene->findTexture(thicknessTexture.Get("index").GetNumberAsInt());
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
    }
}

void GLTFLoader::createGeometry()
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

                auto indexBuffer = acre::createUintBuffer();

                switch (accessor.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                        appendIndex<int8_t>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        appendIndex<uint8_t>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                        appendIndex<int16_t>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        appendIndex<uint16_t>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_INT:
                        appendIndex<int32_t>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        appendIndex<uint32_t>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_FLOAT:
                        appendIndex<float>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                        appendIndex<double>(indexBuffer, accessor, bufferView, geometryIndex, addr);
                        break;
                    default:
                        indexBuffer->data = addr + bufferView.byteOffset + accessor.byteOffset;
                        break;
                }

                indexBuffer->count = accessor.count;
                geometry->index    = m_scene->createVIndexBuffer(indexBuffer);
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

                auto position      = acre::createFloat3Buffer();
                position->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                position->count    = accessor.count;
                geometry->position = m_scene->createVPositionBuffer(position);

                // Evaluate object objBox and scene objBox
                acre::math::box3 box = acre::math::box3::empty();
                for (auto geometryIndex = 0; geometryIndex < accessor.count; geometryIndex += 3)
                {
                    acre::math::float3* pos = (acre::math::float3*)(position->data) + geometryIndex;
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

                auto uv      = acre::createFloat2Buffer();
                uv->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                uv->count    = accessor.count;
                geometry->uv = m_scene->createVUVBuffer(uv);
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
                auto normal      = acre::createFloat3Buffer();
                normal->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                normal->count    = accessor.count;
                geometry->normal = m_scene->createVNormalBuffer(normal);
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
                auto tangent      = acre::createFloat4Buffer();
                tangent->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                tangent->count    = accessor.count;
                geometry->tangent = m_scene->createVTangentBuffer(tangent);
            }

            auto geometryID = m_scene->create(geometry);

            auto key = std::to_string(meshIndex) + "_" + std::to_string(primitiveIndex);
            g_geometry.emplace(key, geometryIndex++);
        }
    }
}

void GLTFLoader::createTransform()
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

        auto transform = acre::createTransform();
        if (!node.matrix.empty())
        {
            transform->matrix = vec16ToFloat4x4(node.matrix);
            transform->affine = acre::math::homogeneousToAffine(transform->matrix);
        }
        else
        {
            transform->affine = affine;
            transform->matrix = acre::math::affineToHomogeneous(affine);
        }
        m_scene->create(transform);
    }

    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        auto node      = m_model->nodes[nodeIndex];
        auto transform = m_scene->getTransform(m_scene->findTransform(nodeIndex));
        for (auto childIndex : node.children)
        {
            auto childTransform = m_scene->getTransform(m_scene->findTransform(childIndex));
            childTransform->matrix *= transform->matrix;
            childTransform->affine *= transform->affine;
        }
    }
}

void GLTFLoader::createComponent()
{
    auto sceneBox = acre::math::box3::empty();
    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        const auto& node = m_model->nodes[nodeIndex];
        if (node.mesh == -1) continue;

        const auto& transformID = nodeIndex;
        const auto& transform   = m_scene->getTransform(m_scene->findTransform(transformID));
        const auto& mesh        = m_model->meshes[node.mesh];
        for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
        {
            auto key = std::to_string(node.mesh) + "_" + std::to_string(primitiveIndex);

            auto entity   = acre::createEntity(key);
            auto entityID = m_scene->create(entity);

            auto geometryIndex = g_geometry[key];
            auto geometryID    = m_scene->findGeometry(geometryIndex);
            auto geometry      = m_scene->getGeometry(geometryID);

            const auto& primitive  = mesh.primitives[primitiveIndex];
            auto        materialID = m_scene->findMaterial(primitive.material);

            m_scene->create(acre::component::createDraw(entityID, geometryID, materialID, transformID));

            auto& objBox = geometry->box;
            objBox       = objBox * transform->affine;
            sceneBox |= objBox;
        }
    }

    m_scene->updateBox(sceneBox);
}
