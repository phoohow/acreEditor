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

static auto bufToFloat4x4(unsigned char* buf)
{
    acre::math::float4x4 mat;
    memcpy(mat.m_data, buf, sizeof(mat));
    return mat;
}

static auto toStride(int componentType, int type, int stride = 0)
{
    int componentCount = 0;
    switch (type)
    {
        case TINYGLTF_TYPE_SCALAR: componentCount = 1; break;
        case TINYGLTF_TYPE_VEC2: componentCount = 2; break;
        case TINYGLTF_TYPE_VEC3: componentCount = 3; break;
        case TINYGLTF_TYPE_VEC4: componentCount = 4; break;
        case TINYGLTF_TYPE_MAT2: componentCount = 4; break;
        case TINYGLTF_TYPE_MAT3: componentCount = 9; break;
        case TINYGLTF_TYPE_MAT4: componentCount = 16; break;
        default: componentCount = 1; break;
    }

    int elementSize = 0;
    switch (componentType)
    {
        case TINYGLTF_COMPONENT_TYPE_BYTE:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: elementSize = 1; break;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: elementSize = 2; break;
        case TINYGLTF_COMPONENT_TYPE_INT:
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_FLOAT: elementSize = 4; break;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE: elementSize = 8; break;
        default: elementSize = 4; break;
    }

    return std::max(componentCount * elementSize, stride);
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
std::map<std::string, int> g_geometry;

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

    _create_sampler();
    _create_material();
    _create_geometry();
    _create_transform();
    _create_skin();
    _create_component_draw();
    _create_animation();
}

void GLTFLoader::_create_sampler()
{
    // {
    //     auto sR = m_scene->create<acre::SamplerID>(0);
    //     auto s  = sR->ptr<acre::SamplerID>();

    //     s->mag_filter = false;
    //     s->min_filter = false;
    //     s->mip_filter = false;
    //     s->address_u  = acre::Sampler::AddressMode::ClampToEdge;
    //     s->address_v  = acre::Sampler::AddressMode::ClampToEdge;
    //     s->address_w  = acre::Sampler::AddressMode::ClampToEdge;

    //     m_scene->update(sR);
    // }

    // {
    //     auto sR = m_scene->create<acre::SamplerID>(1);
    //     auto s  = sR->ptr<acre::SamplerID>();

    //     s->mag_filter = false;
    //     s->min_filter = false;
    //     s->mip_filter = false;
    //     s->address_u  = acre::Sampler::AddressMode::Repeat;
    //     s->address_v  = acre::Sampler::AddressMode::Repeat;
    //     s->address_w  = acre::Sampler::AddressMode::Repeat;

    //     m_scene->update(sR);
    // }

    {
        auto sR = m_scene->create<acre::SamplerID>(0);
        auto s  = sR->ptr<acre::SamplerID>();

        s->mag_filter     = true;
        s->min_filter     = true;
        s->mip_filter     = true;
        s->max_anisotropy = 8;
        s->address_u      = acre::Sampler::AddressMode::Repeat;
        s->address_v      = acre::Sampler::AddressMode::Repeat;
        s->address_w      = acre::Sampler::AddressMode::Repeat;

        m_scene->update(sR);
    }
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

        auto node        = m_scene->create<acre::TextureID>(uuid++);
        auto texture     = node->ptr<acre::TextureID>();
        texture->image   = _get_image_id(refs, tex.source);
        texture->sampler = _get_sampler_id(refs, 0);
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
        auto                base_color = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
        model.base_color               = base_color.xyz();
        material->alpha                = base_color.w;
        model.base_color_idx           = _get_texture_id(refs, mat.pbrMetallicRoughness.baseColorTexture.index);
        model.roughness                = mat.pbrMetallicRoughness.roughnessFactor;
        model.metallic                 = mat.pbrMetallicRoughness.metallicFactor;
        model.metalrough_idx           = _get_texture_id(refs, mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
        model.normal_idx               = _get_texture_id(refs, mat.normalTexture.index);
        model.emission_idx             = _get_texture_id(refs, mat.emissiveTexture.index);
        model.emission                 = vec3ToFloat3(mat.emissiveFactor);

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
            model.use_clearcoat   = true;
            const auto& clearcoat = exts.find("KHR_materials_clearcoat");

            const auto& clearcoatFactor          = clearcoat->second.Get("clearcoatFactor");
            model.clearcoat                      = clearcoatFactor.GetNumberAsDouble();
            const auto& clearcoatRoughnessFactor = clearcoat->second.Get("clearcoatRoughnessFactor");
            model.clearcoat_rough                = clearcoatRoughnessFactor.GetNumberAsDouble();
            if (clearcoat->second.Has("clearcoatTexture"))
            {
                const auto& clearcoatTexture = clearcoat->second.Get("clearcoatTexture");
                auto        index            = clearcoatTexture.Get("index").GetNumberAsInt();
                model.clearcoat_idx          = _get_texture_id(refs, index);
                _try_create_texture_transform(clearcoatTexture, index);
            }
            if (clearcoat->second.Has("clearcoatRoughnessTexture"))
            {
                const auto& clearcoatRoughnessTexture = clearcoat->second.Get("clearcoatRoughnessTexture");
                auto        index                     = clearcoatRoughnessTexture.Get("index").GetNumberAsInt();
                model.clearcoat_rough_idx             = _get_texture_id(refs, index);
                _try_create_texture_transform(clearcoatRoughnessTexture, index);
            }
            if (clearcoat->second.Has("clearcoatNormalTexture"))
            {
                const auto& clearcoatNormalTexture = clearcoat->second.Get("clearcoatNormalTexture");
                auto        index                  = clearcoatNormalTexture.Get("index").GetNumberAsInt();
                model.clearcoat_normal_idx         = _get_texture_id(refs, index);
                _try_create_texture_transform(clearcoatNormalTexture, index);
            }
        }

        if (exts.find("KHR_materials_sheen") != exts.end())
        {
            model.use_sheen   = true;
            const auto& sheen = exts.find("KHR_materials_sheen");

            const auto& sheenColorFactor     = sheen->second.Get("sheenColorFactor");
            model.sheen_color                = acre::math::float3(sheenColorFactor.Get(0).GetNumberAsDouble(),
                                                                  sheenColorFactor.Get(1).GetNumberAsDouble(),
                                                                  sheenColorFactor.Get(2).GetNumberAsDouble());
            const auto& sheenRoughnessFactor = sheen->second.Get("sheenRoughnessFactor");
            model.sheen_roughness            = sheenRoughnessFactor.GetNumberAsDouble();
            if (sheen->second.Has("sheenColorTexture"))
            {
                const auto& sheenColorTexture = sheen->second.Get("sheenColorTexture");
                auto        index             = sheenColorTexture.Get("index").GetNumberAsInt();
                model.sheen_color_idx         = _get_texture_id(refs, index);
                _try_create_texture_transform(sheenColorTexture, index);
            }
            if (sheen->second.Has("sheenRoughnessTexture"))
            {
                const auto& sheenRoughnessTexture = sheen->second.Get("sheenRoughnessTexture");
                auto        index                 = sheenRoughnessTexture.Get("index").GetNumberAsInt();
                model.sheen_rough_idx             = _get_texture_id(refs, index);
                _try_create_texture_transform(sheenRoughnessTexture, index);
            }
        }

        if (exts.find("KHR_materials_anisotropy") != exts.end())
        {
            model.use_anisotropy   = true;
            const auto& anisotropy = exts.find("KHR_materials_anisotropy");

            const auto& anisotropyStrength = anisotropy->second.Get("anisotropyStrength");
            model.anisotropy               = anisotropyStrength.GetNumberAsDouble();

            if (anisotropy->second.Has("anisotropy_rotation"))
            {
                const auto& anisotropy_rotation = anisotropy->second.Get("anisotropy_rotation");
                model.anisotropy_rotation       = anisotropy_rotation.GetNumberAsDouble();
            }
            if (anisotropy->second.Has("anisotropyTexture"))
            {
                const auto& anisotropyTexture = anisotropy->second.Get("anisotropyTexture");
                auto        index             = anisotropyTexture.Get("index").GetNumberAsInt();
                model.anisotropy_idx          = _get_texture_id(refs, index);
                _try_create_texture_transform(anisotropyTexture, index);
            }
        }

        if (exts.find("KHR_materials_iridescence") != exts.end())
        {
            model.use_iridescence   = true;
            const auto& iridescence = exts.find("KHR_materials_iridescence");

            const auto& iridescenceFactor = iridescence->second.Get("iridescenceFactor");
            model.iridescence             = iridescenceFactor.GetNumberAsDouble();
            if (iridescence->second.Has("iridescenceTexture"))
            {
                const auto& iridescenceTexture = iridescence->second.Get("iridescenceTexture");
                auto        index              = iridescenceTexture.Get("index").GetNumberAsInt();
                model.iridescence_idx          = _get_texture_id(refs, index);
                _try_create_texture_transform(iridescenceTexture, index);
            }
            if (iridescence->second.Has("iridescence_ior"))
            {
                const auto& iridescence_ior = iridescence->second.Get("iridescence_ior");
                model.iridescence_ior       = iridescence_ior.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessMaximum"))
            {
                const auto& iridescenceThicknessMaximum = iridescence->second.Get("iridescenceThicknessMaximum");
                model.iridescence_thick_max             = iridescenceThicknessMaximum.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessMinimum"))
            {
                const auto& iridescenceThicknessMinimum = iridescence->second.Get("iridescenceThicknessMinimum");
                model.iridescence_thick_min             = iridescenceThicknessMinimum.GetNumberAsDouble();
            }
            if (iridescence->second.Has("iridescenceThicknessTexture"))
            {
                const auto& iridescenceThicknessTexture = iridescence->second.Get("iridescenceThicknessTexture");
                auto        index                       = iridescenceThicknessTexture.Get("index").GetNumberAsInt();
                model.iridescence_thick_idx             = _get_texture_id(refs, index);
                _try_create_texture_transform(iridescenceThicknessTexture, index);
            }
        }

        const auto& transmission = exts.find("KHR_materials_transmission");
        if (transmission != exts.end())
        {
            model.use_transmission         = true;
            const auto& transmissionFactor = transmission->second.Get("transmissionFactor");

            const auto& value = transmissionFactor.GetNumberAsDouble();
            material->alpha   = value;
            // TODO: test code, complete it later
            material->alpha_idx = _get_texture_id(refs, mat.pbrMetallicRoughness.baseColorTexture.index);
            model.transmission  = transmissionFactor.GetNumberAsDouble();
            if (transmission->second.Has("transmissionTexture"))
            {
                const auto& transmissionTexture = transmission->second.Get("transmissionTexture");
                auto        index               = transmissionTexture.Get("index").GetNumberAsInt();
                model.transmission_idx          = _get_texture_id(refs, index);
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
                    model.thickness_idx          = _get_texture_id(refs, index);
                    _try_create_texture_transform(thicknessTexture, index);
                }

                if (volumeExt->second.Has("attenuation_distance"))
                {
                    const auto& attenuation_distance = volumeExt->second.Get("attenuation_distance");
                    model.attenuation_distance       = attenuation_distance.GetNumberAsDouble();
                }

                if (volumeExt->second.Has("attenuation_color"))
                {
                    const auto& attenuation_color = volumeExt->second.Get("attenuation_color");
                    model.attenuation_color       = acre::math::float3(attenuation_color.Get(0).GetNumberAsDouble(),
                                                                       attenuation_color.Get(1).GetNumberAsDouble(),
                                                                       attenuation_color.Get(2).GetNumberAsDouble());
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

    int geo_idx = 0;
    for (int meshIndex = 0; meshIndex < m_model->meshes.size(); ++meshIndex)
    {
        const auto& mesh = m_model->meshes[meshIndex];
        for (int prim_idx = 0; prim_idx < mesh.primitives.size(); ++prim_idx)
        {
            std::unordered_set<acre::Resource*> refs;

            const auto& primitive = mesh.primitives[prim_idx];
            auto        geo_R     = m_scene->create<acre::GeometryID>(geo_idx);
            auto        geometry  = geo_R->ptr<acre::GeometryID>();

            if (primitive.indices > -1)
            {
                const auto& accessor   = m_model->accessors[primitive.indices];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ||
                    accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    printf("We only support index with ushort or uint\n");
                }

                auto node = m_scene->create<acre::VIndexID>(geo_idx);
                refs.emplace(node);
                auto index_buf    = node->ptr<acre::VIndexID>();
                index_buf->count  = accessor.count;
                index_buf->data   = addr + bufferView.byteOffset + accessor.byteOffset;
                index_buf->stride = toStride(accessor.componentType, accessor.type);
                geometry->index   = node->id<acre::VIndexID>();
            }
            if (primitive.attributes.find("POSITION") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("POSITION")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.type != TINYGLTF_TYPE_VEC3 && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("We only support position with float3\n");
                }

                auto node = m_scene->create<acre::VPositionID>(geo_idx);
                refs.emplace(node);
                auto position      = node->ptr<acre::VPositionID>();
                position->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                position->count    = accessor.count;
                position->stride   = toStride(accessor.componentType, accessor.type);
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

                if (accessor.type != TINYGLTF_TYPE_VEC2 && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("We only support uv with float2\n");
                }

                auto node = m_scene->create<acre::VUVID>(geo_idx);
                refs.emplace(node);
                auto uv      = node->ptr<acre::VUVID>();
                uv->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                uv->count    = accessor.count;
                uv->stride   = toStride(accessor.componentType, accessor.type);
                geometry->uv = node->id<acre::VUVID>();
            }
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("NORMAL")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.type != TINYGLTF_TYPE_VEC3 && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("We only support normal with float3\n");
                }

                auto node = m_scene->create<acre::VNormalID>(geo_idx);
                refs.emplace(node);
                auto normal      = node->ptr<acre::VNormalID>();
                normal->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                normal->count    = accessor.count;
                normal->stride   = toStride(accessor.componentType, accessor.type);
                geometry->normal = node->id<acre::VNormalID>();
            }
            if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("TANGENT")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.type != TINYGLTF_TYPE_VEC4 && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("We only support tangent with float4\n");
                }

                auto node = m_scene->create<acre::VTangentID>(geo_idx);
                refs.emplace(node);
                auto tangent      = node->ptr<acre::VTangentID>();
                tangent->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                tangent->count    = accessor.count;
                tangent->stride   = toStride(accessor.componentType, accessor.type);
                geometry->tangent = node->id<acre::VTangentID>();
            }
            if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("JOINTS_0")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.type != TINYGLTF_TYPE_VEC4 && accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    printf("We only support joints with ushort4\n");
                }

                auto node = m_scene->create<acre::VJointID>(geo_idx);
                refs.emplace(node);
                auto joint      = node->ptr<acre::VJointID>();
                joint->count    = accessor.count;
                joint->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                joint->stride   = toStride(accessor.componentType, accessor.type, bufferView.byteStride);
                geometry->joint = node->id<acre::VJointID>();
            }
            if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
            {
                const auto& accessor   = m_model->accessors[primitive.attributes.find("WEIGHTS_0")->second];
                const auto& bufferView = m_model->bufferViews[accessor.bufferView];
                const auto& addr       = m_model->buffers[bufferView.buffer].data.data();

                if (accessor.type != TINYGLTF_TYPE_VEC4 && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    printf("We only support weights with float4\n");
                }

                auto node = m_scene->create<acre::VWeightID>(geo_idx);
                refs.emplace(node);
                auto weight      = node->ptr<acre::VWeightID>();
                weight->data     = addr + bufferView.byteOffset + accessor.byteOffset;
                weight->count    = accessor.count;
                weight->stride   = toStride(accessor.componentType, accessor.type);
                geometry->weight = node->id<acre::VWeightID>();
            }

            auto key = std::to_string(meshIndex) + "_" + std::to_string(prim_idx);
            g_geometry.emplace(key, geo_idx++);
            m_scene->update(geo_R, std::move(refs));
        }
    }
}

void GLTFLoader::_create_transform()
{
    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        auto trsR = m_scene->create<acre::TransformID>(nodeIndex);
        auto trs  = trsR->ptr<acre::TransformID>();

        auto node   = m_model->nodes[nodeIndex];
        auto affine = acre::math::affine3::identity();
        if (!node.scale.empty())
        {
            trs->scale = vec3ToFloat3(node.scale);
            affine *= acre::math::scaling(trs->scale);
        }
        if (!node.rotation.empty())
        {
            trs->rotation = vec4ToQuat(node.rotation);
            affine *= trs->rotation.toAffine();
        }
        if (!node.translation.empty())
        {
            trs->translation = vec3ToFloat3(node.translation);
            affine *= acre::math::translation(trs->translation);
        }

        if (!node.matrix.empty())
        {
            trs->matrix = vec16ToFloat4x4(node.matrix);
            trs->affine = acre::math::homogeneousToAffine(trs->matrix);
        }
        else
        {
            trs->affine = affine;
            trs->matrix = acre::math::affineToHomogeneous(affine);
        }

        m_scene->update(trsR);
    }

    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        auto node        = m_model->nodes[nodeIndex];
        auto trs         = _get_transform_id(nodeIndex).ptr;
        auto parent_trsR = _get_transform(nodeIndex);
        for (auto childIndex : node.children)
        {
            auto child_trsR = _get_transform(childIndex);
            if (!child_trsR) continue;

            auto child_trs          = child_trsR->ptr<acre::TransformID>();
            auto child_affine_local = acre::math::affine3::identity();
            child_affine_local *= acre::math::scaling(child_trs->scale);
            child_affine_local *= child_trs->rotation.toAffine();
            child_affine_local *= acre::math::translation(child_trs->translation);

            child_trs->affine = child_affine_local * trs->affine;
            child_trs->matrix = acre::math::affineToHomogeneous(child_trs->affine);

            // link parent -> child
            parent_trsR->children.emplace(child_trsR);
            child_trsR->parent = parent_trsR;
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
    m_scene->reset_box();

    auto     sceneBox     = acre::math::box3::empty();
    uint32_t entity_index = 0;
    for (int nodeIndex = 0; nodeIndex < m_model->nodes.size(); ++nodeIndex)
    {
        const auto& node = m_model->nodes[nodeIndex];
        if (node.mesh == -1) continue;

        const auto& trsR = _get_transform(nodeIndex);
        const auto& trs  = trsR->ptr<acre::TransformID>();
        const auto& mesh = m_model->meshes[node.mesh];
        for (int prim_idx = 0; prim_idx < mesh.primitives.size(); ++prim_idx)
        {
            std::unordered_set<acre::Resource*> refs;

            auto key = std::to_string(node.mesh) + "_" + std::to_string(prim_idx);

            // auto entity   = m_scene->create<acre::EntityID>(std::hash<std::string>{}(key));
            auto entity    = m_scene->create<acre::EntityID>(entity_index++);
            auto entity_id = entity->id<acre::EntityID>();

            auto geo_idx = g_geometry[key];
            auto geo_R   = _get_geometry(geo_idx);

            const auto& primitive = mesh.primitives[prim_idx];
            auto        materialR = _get_material(primitive.material);
            if (!materialR)
            {
                materialR        = m_scene->create<acre::MaterialID>(10086);
                auto material    = materialR->ptr<acre::MaterialID>();
                material->type   = acre::MaterialModel::mStandard;
                auto model       = acre::StandardModel();
                model.base_color = acre::math::float3(1.0, 0.0, 0.0);
                material->model  = model;
                m_scene->update(materialR);
            }

            m_scene->create(acre::component::createDraw(entity_id,
                                                        geo_R->id<acre::GeometryID>(),
                                                        materialR->id<acre::MaterialID>(),
                                                        trsR->id<acre::TransformID>()));

            auto& objBox = geo_R->ptr<acre::GeometryID>()->box;
            objBox       = objBox * trs->affine;
            sceneBox |= objBox;

            refs.emplace(geo_R);
            refs.emplace(materialR);
            refs.emplace(trsR);

            m_scene->update(entity, std::move(refs));
        }
    }

    m_scene->merge_box(sceneBox);
}


void GLTFLoader::_create_animation()
{
    auto animationSet = m_scene->animation_set();
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
            acre_channel.target_node = channel.target_node;
            acre_channel.target_path = channel.target_path;
            acre_channel.sampler_idx = channel.sampler;
            acre_animation.channels.push_back(acre_channel);
        }

        animationSet->animations.push_back(acre_animation);
    }
}

void GLTFLoader::_create_texture_transform(const tinygltf::ExtensionMap& ext, uint32_t uuid)
{
    if (ext.find("KHR_texture_transform") == ext.end()) return;

    const auto& trs = ext.find("KHR_texture_transform")->second;
    _create_texture_transform(trs, uuid);
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

    auto acreTrsR   = m_scene->create<acre::TransformID>(textureR->idx() + 1 << 16);
    auto acreTrs    = acreTrsR->ptr<acre::TransformID>();
    acreTrs->matrix = transformMat;
    acreTrs->affine = acre::math::homogeneousToAffine(transformMat);

    textureR->ptr<acre::TextureID>()->transform = acreTrsR->id<acre::TransformID>();

    refs.emplace(acreTrsR);
    m_scene->incRefs(textureR, std::move(refs));
}

acre::Resource* GLTFLoader::_get_image(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    auto node = m_scene->find<acre::ImageID>(uuid);
    refs.emplace(node);
    return node;
}

acre::Resource* GLTFLoader::_get_sampler(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    if (uuid == -1) return nullptr;

    auto node = m_scene->find<acre::SamplerID>(uuid);
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

acre::SamplerID GLTFLoader::_get_sampler_id(std::unordered_set<acre::Resource*>& refs, uint32_t uuid)
{
    return _get_sampler(refs, uuid)->id<acre::SamplerID>();
}
