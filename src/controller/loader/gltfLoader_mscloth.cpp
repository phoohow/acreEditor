#include <controller/loader/gltfLoader.h>
#include <acre/render/renderer.h>

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

void GLTFLoader::_config_mscloth_model(acre::MSCloth& model, float& alpha, acre::TextureID& alpha_idx, std::unordered_set<acre::Resource*>& refs, const tinygltf::Material& mat)
{
    auto base_color     = vec4ToFloat4(mat.pbrMetallicRoughness.baseColorFactor);
    model.rd_albedo     = base_color.xyz();
    alpha               = base_color.w;
    model.rd_albedo_idx = _get_texture_id(refs, mat.pbrMetallicRoughness.baseColorTexture.index);
    model.rs_rough      = mat.pbrMetallicRoughness.roughnessFactor;
    model.rs_rough_idx  = _get_texture_id(refs, mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
    model.normal_idx    = _get_texture_id(refs, mat.normalTexture.index);
    model.rs_albedo_idx = _get_texture_id(refs, mat.emissiveTexture.index);
    model.rs_albedo     = vec3ToFloat3(mat.emissiveFactor);

    _create_texture_transform(mat.pbrMetallicRoughness.baseColorTexture.extensions, mat.pbrMetallicRoughness.baseColorTexture.index);
    _create_texture_transform(mat.pbrMetallicRoughness.metallicRoughnessTexture.extensions, mat.pbrMetallicRoughness.metallicRoughnessTexture.index);
    _create_texture_transform(mat.normalTexture.extensions, mat.normalTexture.index);
    _create_texture_transform(mat.emissiveTexture.extensions, mat.emissiveTexture.index);

    const auto& exts = mat.extensions;

    if (exts.find("KHR_materials_clearcoat") != exts.end())
    {
        const auto& clearcoat = exts.find("KHR_materials_clearcoat");
        if (clearcoat->second.Has("clearcoatRoughnessTexture"))
        {
            const auto& clearcoatRoughnessTexture = clearcoat->second.Get("clearcoatRoughnessTexture");
            auto        index                     = clearcoatRoughnessTexture.Get("index").GetNumberAsInt();
            model.ts_rough_idx                    = _get_texture_id(refs, index);
            _try_create_texture_transform(clearcoatRoughnessTexture, index);
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
        model.sheen_rough                = sheenRoughnessFactor.GetNumberAsDouble();
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

    if (exts.find("KHR_materials_iridescence") != exts.end())
    {
        const auto& iridescence = exts.find("KHR_materials_iridescence");

        const auto& iridescenceFactor = iridescence->second.Get("iridescenceFactor");
        model.ts_albedo               = iridescenceFactor.GetNumberAsDouble();
        if (iridescence->second.Has("iridescenceTexture"))
        {
            const auto& iridescenceTexture = iridescence->second.Get("iridescenceTexture");
            auto        index              = iridescenceTexture.Get("index").GetNumberAsInt();
            model.ts_albedo_idx            = _get_texture_id(refs, index);
            _try_create_texture_transform(iridescenceTexture, index);
        }
        if (iridescence->second.Has("iridescenceThicknessTexture"))
        {
            const auto& iridescenceThicknessTexture = iridescence->second.Get("iridescenceThicknessTexture");
            auto        index                       = iridescenceThicknessTexture.Get("index").GetNumberAsInt();
            model.ts_thick_idx                      = _get_texture_id(refs, index);
            _try_create_texture_transform(iridescenceThicknessTexture, index);
        }
    }

    const auto& transmission = exts.find("KHR_materials_transmission");
    if (transmission != exts.end())
    {
        model.use_transmission         = true;
        const auto& transmissionFactor = transmission->second.Get("transmissionFactor");

        const auto& value = transmissionFactor.GetNumberAsDouble();
        alpha             = value;
        alpha_idx         = _get_texture_id(refs, mat.pbrMetallicRoughness.baseColorTexture.index);
        if (transmission->second.Has("transmissionTexture"))
        {
            const auto& transmissionTexture = transmission->second.Get("transmissionTexture");
            auto        index               = transmissionTexture.Get("index").GetNumberAsInt();
            model.td_albedo_idx             = _get_texture_id(refs, index);
            _try_create_texture_transform(transmissionTexture, index);
        }

        const auto& volumeExt = exts.find("KHR_materials_volume");
        if (volumeExt != exts.end())
        {
            const auto& thicknessFactor = volumeExt->second.Get("thicknessFactor");
            model.rs_thick              = thicknessFactor.GetNumberAsDouble();

            if (volumeExt->second.Has("thicknessTexture"))
            {
                const auto& thicknessTexture = volumeExt->second.Get("thicknessTexture");
                auto        index            = thicknessTexture.Get("index").GetNumberAsInt();
                model.rs_thick_idx           = _get_texture_id(refs, index);
                _try_create_texture_transform(thicknessTexture, index);
            }

            if (volumeExt->second.Has("attenuationDistance"))
            {
                const auto& attenuation_distance = volumeExt->second.Get("attenuationDistance");
                model.td_albedo                  = attenuation_distance.GetNumberAsDouble();
            }

            if (volumeExt->second.Has("attenuationColor"))
            {
                const auto& attenuation_color = volumeExt->second.Get("attenuationColor");
                model.td_albedo               = acre::math::float3(attenuation_color.Get(0).GetNumberAsDouble(),
                                                                   attenuation_color.Get(1).GetNumberAsDouble(),
                                                                   attenuation_color.Get(2).GetNumberAsDouble());
            }
        }
    }
}
