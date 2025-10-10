#include <controller/loader/gltfLoader.h>

#include <acre/render/renderer.h>

#define TINYGLTF_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>

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
static void appendIndex(acre::VIndex*               indexBuffer,
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

acre::TextureID GLTFLoader::_get_texture(uint32_t uuid)
{
    if (uuid == -1) return acre::TextureID();

    auto node = m_scene->find<acre::TextureID>(uuid);
    return node->id<acre::TextureID>();
}

acre::TransformID GLTFLoader::_get_transform(uint32_t uuid)
{
    if (uuid == -1) return acre::TransformID();

    auto node = m_scene->find<acre::TransformID>(uuid);
    return node->id<acre::TransformID>();
}

acre::GeometryID GLTFLoader::_get_geometry(uint32_t uuid)
{
    if (uuid == -1) return acre::GeometryID();

    auto node = m_scene->find<acre::GeometryID>(uuid);
    return node->id<acre::GeometryID>();
}

acre::MaterialID GLTFLoader::_get_material(uint32_t uuid)
{
    if (uuid == -1) return acre::MaterialID();

    auto node = m_scene->find<acre::MaterialID>(uuid);
    return node->id<acre::MaterialID>();
}

void GLTFLoader::createMaterial()
{
    uint32_t uuid = 0;
    for (const auto& img : m_model->images)
    {
        auto node     = m_scene->create<acre::ImageID>(uuid++);
        auto image    = node->id<acre::ImageID>().ptr;
        image->data   = (void*)img.image.data();
        image->width  = img.width;
        image->height = img.height;
        image->name   = img.name.c_str();
        image->format = toImageFormat(img.component, img.bits);
    }

    uuid = 0;
    for (auto& tex : m_model->textures)
    {
        auto node      = m_scene->create<acre::TextureID>(uuid++);
        auto texture   = node->id<acre::TextureID>().ptr;
        texture->image = m_scene->find<acre::ImageID>(tex.source)->id<acre::ImageID>();
    }

    uuid = 0;
    for (const auto& mat : m_model->materials)
    {
        auto node      = m_scene->create<acre::MaterialID>(uuid++);
        auto material  = node->id<acre::MaterialID>().ptr;
        material->type = acre::MaterialModel::mStandard;
        acre::StandardModel model;
        auto                baseColor = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
        model.baseColor               = baseColor.xyz();
        material->alpha               = baseColor.w;
        model.baseColorIndex          = _get_texture(mat.pbrMetallicRoughness.baseColorTexture.index);
        model.roughness               = mat.pbrMetallicRoughness.roughnessFactor;
        model.metallic                = mat.pbrMetallicRoughness.metallicFactor;
        model.metalRoughIndex         = _get_texture(mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
        model.normalIndex             = _get_texture(mat.normalTexture.index);
        model.emission                = vec3ToFloat3(mat.emissiveFactor);
        model.emissionIndex           = _get_texture(mat.emissiveTexture.index);

        createTextureTransform(mat.pbrMetallicRoughness.baseColorTexture.extensions, model.baseColorIndex);
        createTextureTransform(mat.pbrMetallicRoughness.metallicRoughnessTexture.extensions, model.metalRoughIndex);
        createTextureTransform(mat.normalTexture.extensions, model.normalIndex);
        createTextureTransform(mat.emissiveTexture.extensions, model.emissionIndex);

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
                model.clearcoatIndex         = _get_texture(clearcoatTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(clearcoatTexture, model.clearcoatIndex);
            }
            if (clearcoat->second.Has("clearcoatRoughnessTexture"))
            {
                const auto& clearcoatRoughnessTexture = clearcoat->second.Get("clearcoatRoughnessTexture");
                model.clearcoatRoughnessIndex         = _get_texture(clearcoatRoughnessTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(clearcoatRoughnessTexture, model.clearcoatRoughnessIndex);
            }
            if (clearcoat->second.Has("clearcoatNormalTexture"))
            {
                const auto& clearcoatNormalTexture = clearcoat->second.Get("clearcoatNormalTexture");
                model.clearcoatNormalIndex         = _get_texture(clearcoatNormalTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(clearcoatNormalTexture, model.clearcoatNormalIndex);
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
                model.sheenColorIndex         = _get_texture(sheenColorTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(sheenColorTexture, model.sheenColorIndex);
            }
            if (sheen->second.Has("sheenRoughnessTexture"))
            {
                const auto& sheenRoughnessTexture = sheen->second.Get("sheenRoughnessTexture");
                model.sheenRoughnessIndex         = _get_texture(sheenRoughnessTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(sheenRoughnessTexture, model.sheenRoughnessIndex);
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
                model.anisotropyIndex         = _get_texture(anisotropyTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(anisotropyTexture, model.anisotropyIndex);
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
                model.iridescenceIndex         = _get_texture(iridescenceTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(iridescenceTexture, model.iridescenceIndex);
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
                model.iridescenceThicknessIndex         = _get_texture(iridescenceThicknessTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(iridescenceThicknessTexture, model.iridescenceThicknessIndex);
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
            material->alphaIndex = _get_texture(mat.pbrMetallicRoughness.baseColorTexture.index);
            model.transmission   = transmissionFactor.GetNumberAsDouble();
            if (transmission->second.Has("transmissionTexture"))
            {
                const auto& transmissionTexture = transmission->second.Get("transmissionTexture");
                model.transmissionIndex         = _get_texture(transmissionTexture.Get("index").GetNumberAsInt());
                checkCreateTextureTransform(transmissionTexture, model.transmissionIndex);
            }

            const auto& volumeExt = exts.find("KHR_materials_volume");
            if (volumeExt != exts.end())
            {
                const auto& thicknessFactor = volumeExt->second.Get("thicknessFactor");
                model.thickness             = thicknessFactor.GetNumberAsDouble();

                if (volumeExt->second.Has("thicknessTexture"))
                {
                    const auto& thicknessTexture = volumeExt->second.Get("thicknessTexture");
                    model.thicknessIndex         = _get_texture(thicknessTexture.Get("index").GetNumberAsInt());
                    checkCreateTextureTransform(thicknessTexture, model.thicknessIndex);
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
            auto        geometryR = m_scene->create<acre::GeometryID>(geometryIndex);
            auto        geometry  = geometryR->id<acre::GeometryID>().ptr;

            if (primitive.indices > -1)
            {
                const auto& accessor   = m_model->accessors[primitive.indices];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                auto node          = m_scene->create<acre::VIndexID>(geometryIndex);
                auto indexBuffer   = node->id<acre::VIndexID>().ptr;
                indexBuffer->count = accessor.count;
                geometry->index    = node->id<acre::VIndexID>();

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

                auto node          = m_scene->create<acre::VPositionID>(geometryIndex);
                auto position      = node->id<acre::VPositionID>().ptr;
                position->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                position->count    = accessor.count;
                geometry->position = node->id<acre::VPositionID>();

                // Evaluate object objBox and scene objBox
                acre::math::box3 box = acre::math::box3::empty();
                for (auto i = 0; i < accessor.count; i += 3)
                {
                    acre::math::float3* pos = (acre::math::float3*)(position->data) + i;
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

                auto node    = m_scene->create<acre::VUVID>(geometryIndex);
                auto uv      = node->id<acre::VUVID>().ptr;
                uv->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                uv->count    = accessor.count;
                geometry->uv = node->id<acre::VUVID>();
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

                auto node        = m_scene->create<acre::VNormalID>(geometryIndex);
                auto normal      = node->id<acre::VNormalID>().ptr;
                normal->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                normal->count    = accessor.count;
                geometry->normal = node->id<acre::VNormalID>();
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

                auto node         = m_scene->create<acre::VTangentID>(geometryIndex);
                auto tangent      = node->id<acre::VTangentID>().ptr;
                tangent->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                tangent->count    = accessor.count;
                geometry->tangent = node->id<acre::VTangentID>();
            }

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

        auto transformR = m_scene->create<acre::TransformID>(nodeIndex);
        auto transform  = transformR->id<acre::TransformID>().ptr;
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
    }

    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        auto node      = m_model->nodes[nodeIndex];
        auto transform = _get_transform(nodeIndex).ptr;
        for (auto childIndex : node.children)
        {
            auto childTransform = _get_transform(childIndex).ptr;
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

        const auto& transformID = _get_transform(nodeIndex);
        const auto& transform   = transformID.ptr;
        const auto& mesh        = m_model->meshes[node.mesh];
        for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
        {
            auto key = std::to_string(node.mesh) + "_" + std::to_string(primitiveIndex);

            auto entity   = m_scene->create<acre::EntityID>(std::hash<std::string>{}(key));
            auto entityID = entity->id<acre::EntityID>();

            auto geometryIndex = g_geometry[key];
            auto geometryID    = _get_geometry(geometryIndex);

            const auto& primitive  = mesh.primitives[primitiveIndex];
            auto        materialID = _get_material(primitive.material);

            m_scene->create(acre::component::createDraw(entityID, geometryID, materialID, transformID));

            auto& objBox = geometryID.ptr->box;
            objBox       = objBox * transform->affine;
            sceneBox |= objBox;
        }
    }

    m_scene->mergeBox(sceneBox);
}

void GLTFLoader::createTextureTransform(const tinygltf::ExtensionMap& ext, acre::TextureID textureID)
{
    if (ext.find("KHR_texture_transform") == ext.end()) return;

    const auto& transform = ext.find("KHR_texture_transform")->second;
    createTextureTransform(transform, textureID);
}

void GLTFLoader::checkCreateTextureTransform(const tinygltf::Value& value, acre::TextureID textureID)
{
    if (!value.Has("extensions")) return;

    const auto& extensions = value.Get("extensions");
    if (!extensions.Has("KHR_texture_transform")) return;

    createTextureTransform(extensions.Get("KHR_texture_transform"), textureID);
}

void GLTFLoader::createTextureTransform(const tinygltf::Value& value, acre::TextureID textureID)
{
    acre::math::float4x4 transformMat = acre::math::float4x4::identity();
    if (value.Has("scale"))
    {
        acre::math::float2 scale(1.0f, 1.0f);
        const auto&        scale_ = value.Get("scale");
        scale.x                   = scale_.Get(0).GetNumberAsDouble();
        scale.y                   = scale_.Get(1).GetNumberAsDouble();

        acre::math::float4x4 scaleMat = acre::math::float4x4::identity();
        scaleMat.m00                  = scale.x;
        scaleMat.m11                  = scale.y;

        transformMat *= scaleMat;
    }
    if (value.Has("rotation"))
    {
        float rotation = 0.0f;
        rotation       = value.Get("rotation").GetNumberAsDouble();

        acre::math::float4x4 rotationMat = acre::math::float4x4::identity();
        rotationMat.m00                  = cos(rotation);
        rotationMat.m01                  = -sin(rotation);
        rotationMat.m10                  = sin(rotation);
        rotationMat.m11                  = cos(rotation);

        transformMat *= rotationMat;
    }
    if (value.Has("offset"))
    {
        acre::math::float2 offset(0.0f, 0.0f);
        const auto&        offset_ = value.Get("offset");
        offset.x                   = offset_.Get(0).GetNumberAsDouble();
        offset.y                   = offset_.Get(1).GetNumberAsDouble();

        acre::math::float4x4 translation = acre::math::float4x4::identity();
        translation.m30                  = offset.x;
        translation.m31                  = offset.y;

        transformMat *= translation;
    }

    auto acreTransformR   = m_scene->create<acre::TransformID>(textureID.idx + 1 << 16);
    auto acreTransform    = acreTransformR->id<acre::TransformID>().ptr;
    acreTransform->matrix = transformMat;
    acreTransform->affine = acre::math::homogeneousToAffine(transformMat);

    textureID.ptr->transform = acreTransformR->id<acre::TransformID>();
}
