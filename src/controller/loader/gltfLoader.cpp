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

// clang-format off
static auto bufToFloat4x4(unsigned char* buf)
{
    acre::math::float4x4 mat;
    memcpy(mat.m_data, buf, sizeof(mat));

    // mat.m00 = float(*buf); buf += 4;
    // mat.m01 = float(*buf); buf += 4;
    // mat.m02 = float(*buf); buf += 4;
    // mat.m03 = float(*buf); buf += 4;

    // mat.m10 = float(*buf); buf += 4;
    // mat.m11 = float(*buf); buf += 4;
    // mat.m12 = float(*buf); buf += 4;
    // mat.m13 = float(*buf); buf += 4;

    // mat.m20 = float(*buf); buf += 4;
    // mat.m21 = float(*buf); buf += 4;
    // mat.m22 = float(*buf); buf += 4;
    // mat.m23 = float(*buf); buf += 4;

    // mat.m30 = float(*buf); buf += 4;
    // mat.m31 = float(*buf); buf += 4;
    // mat.m32 = float(*buf); buf += 4;
    // mat.m33 = float(*buf); 

    return mat;
}
// clang-format on

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
std::vector<std::vector<uint32_t>> g_joint;
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

template <typename Type>
static void appendJoint(acre::VJoint*               indexBuffer,
                        const tinygltf::Accessor&   accessor,
                        const tinygltf::BufferView& bufferView,
                        uint32_t                    index,
                        unsigned char*              addr)
{
    if (accessor.type != TINYGLTF_TYPE_VEC4)
    {
        printf("JOINTS_0 accessor.type is not VEC4!\n");
        return;
    }
    size_t               jointCount = accessor.count * 4;
    const unsigned char* src        = addr + bufferView.byteOffset + accessor.byteOffset;
    std::vector<Type>    tempVec(jointCount);
    memcpy(tempVec.data(), src, sizeof(Type) * jointCount);

    g_joint[index].resize(jointCount);
    for (size_t i = 0; i < jointCount; ++i)
        g_joint[index][i] = static_cast<uint32_t>(tempVec[i]);

    indexBuffer->data = g_joint[index].data();
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

    _create_material();
    _create_geometry();
    _create_transform();
    _create_skin();
    _create_component_draw();
    _create_animation();
}

void GLTFLoader::_create_material()
{
    uint32_t uuid = 0;
    for (const auto& img : m_model->images)
    {
        auto node     = m_scene->create<acre::ImageID>(uuid++);
        auto image    = node->ptr<acre::ImageID>();
        image->data   = (void*)img.image.data();
        image->width  = img.width;
        image->height = img.height;
        image->name   = img.name.c_str();
        image->format = toImageFormat(img.component, img.bits);
    }

    uuid = 0;
    for (auto& tex : m_model->textures)
    {
        std::unordered_set<acre::Resource*> refs;

        auto node      = m_scene->create<acre::TextureID>(uuid++);
        auto texture   = node->ptr<acre::TextureID>();
        texture->image = _get_image_id(refs, tex.source);
        m_scene->update(node, std::move(refs));
    }

    uuid = 0;
    for (const auto& mat : m_model->materials)
    {
        std::unordered_set<acre::Resource*> refs;

        auto materialR = m_scene->create<acre::MaterialID>(uuid++);
        auto material  = materialR->ptr<acre::MaterialID>();
        material->type = acre::MaterialModel::mStandard;
        acre::StandardModel model;
        auto                baseColor = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
        model.baseColor               = baseColor.xyz();
        material->alpha               = baseColor.w;
        model.baseColorIndex          = _get_texture_id(refs, mat.pbrMetallicRoughness.baseColorTexture.index);
        model.roughness               = mat.pbrMetallicRoughness.roughnessFactor;
        model.metallic                = mat.pbrMetallicRoughness.metallicFactor;
        model.metalRoughIndex         = _get_texture_id(refs, mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
        model.normalIndex             = _get_texture_id(refs, mat.normalTexture.index);
        model.emissionIndex           = _get_texture_id(refs, mat.emissiveTexture.index);
        model.emission                = vec3ToFloat3(mat.emissiveFactor);

        _create_texture_transform(mat.pbrMetallicRoughness.baseColorTexture.extensions, mat.pbrMetallicRoughness.baseColorTexture.index);
        _create_texture_transform(mat.pbrMetallicRoughness.metallicRoughnessTexture.extensions, mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
        _create_texture_transform(mat.normalTexture.extensions, mat.normalTexture.index);
        _create_texture_transform(mat.emissiveTexture.extensions, mat.emissiveTexture.index);

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
                auto        index            = clearcoatTexture.Get("index").GetNumberAsInt();
                model.clearcoatIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(clearcoatTexture, index);
            }
            if (clearcoat->second.Has("clearcoatRoughnessTexture"))
            {
                const auto& clearcoatRoughnessTexture = clearcoat->second.Get("clearcoatRoughnessTexture");
                auto        index                     = clearcoatRoughnessTexture.Get("index").GetNumberAsInt();
                model.clearcoatRoughnessIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(clearcoatRoughnessTexture, index);
            }
            if (clearcoat->second.Has("clearcoatNormalTexture"))
            {
                const auto& clearcoatNormalTexture = clearcoat->second.Get("clearcoatNormalTexture");
                auto        index                  = clearcoatNormalTexture.Get("index").GetNumberAsInt();
                model.clearcoatNormalIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(clearcoatNormalTexture, index);
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
                auto        index             = sheenColorTexture.Get("index").GetNumberAsInt();
                model.sheenColorIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(sheenColorTexture, index);
            }
            if (sheen->second.Has("sheenRoughnessTexture"))
            {
                const auto& sheenRoughnessTexture = sheen->second.Get("sheenRoughnessTexture");
                auto        index                 = sheenRoughnessTexture.Get("index").GetNumberAsInt();
                model.sheenRoughnessIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(sheenRoughnessTexture, index);
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
                auto        index             = anisotropyTexture.Get("index").GetNumberAsInt();
                model.anisotropyIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(anisotropyTexture, index);
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
                auto        index              = iridescenceTexture.Get("index").GetNumberAsInt();
                model.iridescenceIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(iridescenceTexture, index);
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
                auto        index                       = iridescenceThicknessTexture.Get("index").GetNumberAsInt();
                model.iridescenceThicknessIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(iridescenceThicknessTexture, index);
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
            material->alphaIndex = _get_texture_id(refs, mat.pbrMetallicRoughness.baseColorTexture.index);
            model.transmission   = transmissionFactor.GetNumberAsDouble();
            if (transmission->second.Has("transmissionTexture"))
            {
                const auto& transmissionTexture = transmission->second.Get("transmissionTexture");
                auto        index               = transmissionTexture.Get("index").GetNumberAsInt();
                model.transmissionIndex         = _get_texture_id(refs, index);
                _try_create_texture_transform(transmissionTexture, index);
            }

            const auto& volumeExt = exts.find("KHR_materials_volume");
            if (volumeExt != exts.end())
            {
                const auto& thicknessFactor = volumeExt->second.Get("thicknessFactor");
                model.thickness             = thicknessFactor.GetNumberAsDouble();

                if (volumeExt->second.Has("thicknessTexture"))
                {
                    const auto& thicknessTexture = volumeExt->second.Get("thicknessTexture");
                    auto        index            = thicknessTexture.Get("index").GetNumberAsInt();
                    model.thicknessIndex         = _get_texture_id(refs, index);
                    _try_create_texture_transform(thicknessTexture, index);
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
        m_scene->update(materialR, std::move(refs));
    }
}

void GLTFLoader::_create_geometry()
{
    g_geometry.clear();

    int geometryIndex = 0;
    for (int meshIndex = 0; meshIndex < m_model->meshes.size(); ++meshIndex)
    {
        const auto& mesh = m_model->meshes[meshIndex];
        g_index.resize(g_index.size() + mesh.primitives.size());
        g_joint.resize(g_joint.size() + mesh.primitives.size());

        for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
        {
            std::unordered_set<acre::Resource*> refs;

            const auto& primitive = mesh.primitives[primitiveIndex];
            auto        geometryR = m_scene->create<acre::GeometryID>(geometryIndex);
            auto        geometry  = geometryR->ptr<acre::GeometryID>();

            if (primitive.indices > -1)
            {
                const auto& accessor   = m_model->accessors[primitive.indices];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                auto node = m_scene->create<acre::VIndexID>(geometryIndex);
                refs.emplace(node);
                auto indexBuffer   = node->ptr<acre::VIndexID>();
                indexBuffer->count = accessor.count;
                geometry->index    = node->id<acre::VIndexID>();

                switch (accessor.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_BYTE: appendIndex<int8_t>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: appendIndex<uint8_t>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_SHORT: appendIndex<int16_t>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: appendIndex<uint16_t>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_INT: appendIndex<int32_t>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: appendIndex<uint32_t>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_FLOAT: appendIndex<float>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    case TINYGLTF_COMPONENT_TYPE_DOUBLE: appendIndex<double>(indexBuffer, accessor, bufferView, geometryIndex, addr); break;
                    default: indexBuffer->data = addr + bufferView.byteOffset + accessor.byteOffset; break;
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

                auto node = m_scene->create<acre::VPositionID>(geometryIndex);
                refs.emplace(node);
                auto position      = node->ptr<acre::VPositionID>();
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

                auto node = m_scene->create<acre::VUVID>(geometryIndex);
                refs.emplace(node);
                auto uv      = node->ptr<acre::VUVID>();
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

                auto node = m_scene->create<acre::VNormalID>(geometryIndex);
                refs.emplace(node);
                auto normal      = node->ptr<acre::VNormalID>();
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

                auto node = m_scene->create<acre::VTangentID>(geometryIndex);
                refs.emplace(node);
                auto tangent      = node->ptr<acre::VTangentID>();
                tangent->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                tangent->count    = accessor.count;
                geometry->tangent = node->id<acre::VTangentID>();
            }
            if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("JOINTS_0")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.type != TINYGLTF_TYPE_VEC4 || accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    printf("Undo!\n");
                }

                auto node = m_scene->create<acre::VJointID>(geometryIndex);
                refs.emplace(node);
                auto joint      = node->ptr<acre::VJointID>();
                joint->count    = accessor.count;
                joint->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                geometry->joint = node->id<acre::VJointID>();

                // switch (accessor.componentType)
                // {
                //     case TINYGLTF_COMPONENT_TYPE_BYTE: appendJoint<int8_t>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: appendJoint<uint8_t>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_SHORT: appendJoint<int16_t>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: appendJoint<uint16_t>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_INT: appendJoint<int32_t>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: appendJoint<uint32_t>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_FLOAT: appendJoint<float>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     case TINYGLTF_COMPONENT_TYPE_DOUBLE: appendJoint<double>(joint, accessor, bufferView, geometryIndex, addr); break;
                //     default: joint->data = addr + bufferView.byteOffset + accessor.byteOffset; break;
                // }
            }
            if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("WEIGHTS_0")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("Undo!\n");
                }

                auto node = m_scene->create<acre::VWeightID>(geometryIndex);
                refs.emplace(node);
                auto weight      = node->ptr<acre::VWeightID>();
                weight->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                weight->count    = accessor.count;
                geometry->weight = node->id<acre::VWeightID>();
            }

            auto key = std::to_string(meshIndex) + "_" + std::to_string(primitiveIndex);
            g_geometry.emplace(key, geometryIndex++);
            m_scene->update(geometryR, std::move(refs));
        }
    }
}

void GLTFLoader::_create_transform()
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
        auto transform  = transformR->ptr<acre::TransformID>();
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
        auto transform = _get_transform_id(nodeIndex).ptr;
        for (auto childIndex : node.children)
        {
            auto childTransform = _get_transform_id(childIndex).ptr;
            childTransform->matrix *= transform->matrix;
            childTransform->affine *= transform->affine;
        }
    }
}

void GLTFLoader::_create_skin()
{
    for (int node_idx = 0; node_idx < m_model->nodes.size(); ++node_idx)
    {
        auto node = m_model->nodes[node_idx];
        if (node.mesh == -1 || node.skin == -1) continue;

        auto node_transform      = _get_transform_id(node_idx).ptr;
        auto inverse_node_matrix = acre::math::inverse(node_transform->matrix);

        auto skin     = m_model->skins[node.skin];
        auto accessor = m_model->accessors[skin.inverseBindMatrices];
        auto view     = m_model->bufferViews[accessor.bufferView];
        auto buffer   = m_model->buffers[view.buffer].data;

        for (int joint_idx = 0; joint_idx < skin.joints.size(); ++joint_idx)
        {
            auto addr                = buffer.data() + accessor.byteOffset + view.byteOffset + joint_idx * 64;
            auto inverse_bind_matrix = bufToFloat4x4(addr);

            auto joint_node_idx    = skin.joints[joint_idx];
            auto joint_node_matrix = _get_transform(joint_node_idx)->ptr<acre::TransformID>()->matrix;

            auto skinR                                      = m_scene->create<acre::SkinID>(joint_node_idx);
            skinR->ptr<acre::SkinID>()->inverse_bind_matrix = inverse_bind_matrix;
            skinR->ptr<acre::SkinID>()->inverse_node_matrix = inverse_node_matrix;
            skinR->ptr<acre::SkinID>()->joint_matrix        = inverse_bind_matrix * joint_node_matrix * inverse_node_matrix;
            skinR->ptr<acre::SkinID>()->joint_affine        = acre::math::homogeneousToAffine(skinR->ptr<acre::SkinID>()->joint_matrix);
        }
    }
}

void GLTFLoader::_create_component_draw()
{
    auto     sceneBox     = acre::math::box3::empty();
    uint32_t entity_index = 0;
    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        const auto& node = m_model->nodes[nodeIndex];
        if (node.mesh == -1) continue;

        const auto& transformR = _get_transform(nodeIndex);
        const auto& transform  = transformR->ptr<acre::TransformID>();
        const auto& mesh       = m_model->meshes[node.mesh];
        for (int primitiveIndex = 0; primitiveIndex < mesh.primitives.size(); ++primitiveIndex)
        {
            std::unordered_set<acre::Resource*> refs;

            auto key = std::to_string(node.mesh) + "_" + std::to_string(primitiveIndex);

            // auto entity   = m_scene->create<acre::EntityID>(std::hash<std::string>{}(key));
            auto entity   = m_scene->create<acre::EntityID>(entity_index++);
            auto entityID = entity->id<acre::EntityID>();

            auto geometryIndex = g_geometry[key];
            auto geometryR     = _get_geometry(geometryIndex);

            const auto& primitive = mesh.primitives[primitiveIndex];
            auto        materialR = _get_material(primitive.material);
            if (!materialR)
            {
                materialR       = m_scene->create<acre::MaterialID>(10086);
                auto material   = materialR->ptr<acre::MaterialID>();
                material->type  = acre::MaterialModel::mStandard;
                auto model      = acre::StandardModel();
                model.baseColor = acre::math::float3(1.0, 0.0, 0.0);
                material->model = model;
                m_scene->update(materialR);
            }

            m_scene->create(acre::component::createDraw(entityID,
                                                        geometryR->id<acre::GeometryID>(),
                                                        materialR->id<acre::MaterialID>(),
                                                        transformR->id<acre::TransformID>()));

            auto& objBox = geometryR->ptr<acre::GeometryID>()->box;
            objBox       = objBox * transform->affine;
            sceneBox |= objBox;

            refs.emplace(geometryR);
            refs.emplace(materialR);
            refs.emplace(transformR);

            m_scene->update(entity, std::move(refs));
        }
    }

    m_scene->mergeBox(sceneBox);
}


void GLTFLoader::_create_animation()
{
    auto animationSet = m_scene->getAnimationSet();
    for (const auto& animation : m_model->animations)
    {
        acre::Animation acre_animation;
        acre_animation.name     = animation.name;
        acre_animation.duration = 0.0f;

        // samplers
        for (const auto& sampler : animation.samplers)
        {
            acre::AnimationSampler acre_sampler;
            acre_sampler.interpolation = sampler.interpolation;

            // input
            const auto&  inputAccessor   = m_model->accessors[sampler.input];
            const auto&  inputBufferView = m_model->bufferViews[inputAccessor.bufferView];
            const auto&  inputBuffer     = m_model->buffers[inputBufferView.buffer].data;
            const float* inputData       = reinterpret_cast<const float*>(&inputBuffer[inputBufferView.byteOffset + inputAccessor.byteOffset]);
            acre_sampler.input.assign(inputData, inputData + inputAccessor.count);

            // output
            const auto&  outputAccessor   = m_model->accessors[sampler.output];
            const auto&  outputBufferView = m_model->bufferViews[outputAccessor.bufferView];
            const auto&  outputBuffer     = m_model->buffers[outputBufferView.buffer].data;
            const float* outputData       = reinterpret_cast<const float*>(&outputBuffer[outputBufferView.byteOffset + outputAccessor.byteOffset]);
            int          elemSize         = 1;
            if (outputAccessor.type == TINYGLTF_TYPE_VEC3)
                elemSize = 3;
            else if (outputAccessor.type == TINYGLTF_TYPE_VEC4)
                elemSize = 4;
            for (size_t i = 0; i < outputAccessor.count; ++i)
            {
                std::vector<float> value(outputData + i * elemSize, outputData + (i + 1) * elemSize);
                acre_sampler.output.push_back(value);
            }
            acre_animation.samplers.push_back(acre_sampler);
            if (!acre_sampler.input.empty() && acre_sampler.input.back() > acre_animation.duration)
                acre_animation.duration = acre_sampler.input.back();
        }

        // channels
        for (const auto& channel : animation.channels)
        {
            acre::AnimationChannel acre_channel;
            acre_channel.targetNode   = channel.target_node;
            acre_channel.targetPath   = channel.target_path;
            acre_channel.samplerIndex = channel.sampler;
            acre_animation.channels.push_back(acre_channel);
        }

        animationSet->animations.push_back(acre_animation);
    }
}

void GLTFLoader::_create_texture_transform(const tinygltf::ExtensionMap& ext, uint32_t uuid)
{
    if (ext.find("KHR_texture_transform") == ext.end()) return;

    const auto& transform = ext.find("KHR_texture_transform")->second;
    _create_texture_transform(transform, uuid);
}

void GLTFLoader::_try_create_texture_transform(const tinygltf::Value& value, uint32_t uuid)
{
    if (!value.Has("extensions")) return;

    const auto& extensions = value.Get("extensions");
    if (!extensions.Has("KHR_texture_transform")) return;

    _create_texture_transform(extensions.Get("KHR_texture_transform"), uuid);
}

void GLTFLoader::_create_texture_transform(const tinygltf::Value& value, uint32_t uuid)
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

    std::unordered_set<acre::Resource*> refs;

    auto textureR = m_scene->find<acre::TextureID>(uuid);

    auto acreTransformR   = m_scene->create<acre::TransformID>(textureR->idx() + 1 << 16);
    auto acreTransform    = acreTransformR->ptr<acre::TransformID>();
    acreTransform->matrix = transformMat;
    acreTransform->affine = acre::math::homogeneousToAffine(transformMat);

    textureR->ptr<acre::TextureID>()->transform = acreTransformR->id<acre::TransformID>();

    refs.emplace(acreTransformR);
    m_scene->incRefs(textureR, std::move(refs));
}

acre::Resource* GLTFLoader::_get_image(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    auto node = m_scene->find<acre::ImageID>(uuid);
    refs.emplace(node);
    return node;
}

acre::Resource* GLTFLoader::_get_texture(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    auto node = m_scene->find<acre::TextureID>(uuid);
    refs.emplace(node);
    return node;
}

acre::Resource* GLTFLoader::_get_transform(uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    return m_scene->find<acre::TransformID>(uuid);
}

acre::Resource* GLTFLoader::_get_geometry(uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    return m_scene->find<acre::GeometryID>(uuid);
}

acre::Resource* GLTFLoader::_get_material(uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    return m_scene->find<acre::MaterialID>(uuid);
}

acre::ImageID GLTFLoader::_get_image_id(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    return _get_image(refs, uuid)->id<acre::ImageID>();
}

acre::TextureID GLTFLoader::_get_texture_id(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    auto node = _get_texture(refs, uuid);
    if (!node) return acre::TextureID();

    return node->id<acre::TextureID>();
}

acre::TransformID GLTFLoader::_get_transform_id(uint32_t uuid)
{
    return _get_transform(uuid)->id<acre::TransformID>();
}

acre::GeometryID GLTFLoader::_get_geometry_id(uint32_t uuid)
{
    return _get_geometry(uuid)->id<acre::GeometryID>();
}

acre::MaterialID GLTFLoader::_get_material_id(uint32_t uuid)
{
    return _get_material(uuid)->id<acre::MaterialID>();
}
