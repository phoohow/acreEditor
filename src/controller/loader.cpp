#include <controller/loader.h>

#include <stb/stb_image.h>
#include <fstream>
#include <sstream>

static auto splitSimpleText(const std::string& line)
{
    std::istringstream       iss(line);
    std::vector<std::string> tokens;
    std::string              token;

    while (iss >> token)
        tokens.push_back(token);

    return tokens;
}

static auto splitCameraParameter(const std::string& line)
{
    std::stringstream        ss(line);
    std::vector<std::string> tokens;
    std::string              token;

    while (std::getline(ss, token, ':'))
        tokens.push_back(token);

    return tokens;
}

Loader::Loader(SceneMgr* scene) :
    m_scene(scene) {}

acre::Resource* Loader::createImage(const std::string& fileName)
{
    auto node  = m_scene->create<acre::ImageID>(std::hash<std::string>{}(fileName));
    auto image = node->ptr<acre::ImageID>();

    int  width;
    int  height;
    int  channels;
    int  desired   = 4;
    auto imageData = stbi_loadf(fileName.c_str(), &width, &height, &channels, desired);
    if (!imageData) return nullptr;

    image->name    = fileName.c_str();
    image->data    = (imageData);
    image->width   = width;
    image->height  = height;
    image->format  = desired == 3 ? acre::Image::Format::RGB32_FLOAT : acre::Image::Format::RGBA32_FLOAT;
    image->mipmaps = log2(width >= height ? width : height);

    return node;
}

void Loader::loadImage(const std::string& fileName)
{
    auto node    = m_scene->create<acre::TextureID>(std::hash<std::string>{}(fileName));
    auto texture = node->ptr<acre::TextureID>();

    auto imageR    = createImage(fileName);
    texture->image = imageR->id<acre::ImageID>();
}

void Loader::loadHDR(const std::string& fileName)
{
    auto node    = m_scene->create<acre::TextureID>(std::hash<std::string>{}(fileName));
    auto texture = node->ptr<acre::TextureID>();

    auto imageR    = createImage(fileName);
    texture->image = imageR->id<acre::ImageID>();

    auto light    = new acre::HDRLight;
    light->id     = node->id<acre::TextureID>();
    light->enable = true;

    m_scene->setHDRLight(light);
}

void Loader::loadLutGGX(const std::string& fileName)
{
    auto node    = m_scene->create<acre::TextureID>(std::hash<std::string>{}(fileName));
    auto texture = node->ptr<acre::TextureID>();

    auto imageR    = createImage(fileName);
    texture->image = imageR->id<acre::ImageID>();

    m_scene->setLutGGX(node->id<acre::TextureID>());
}

void Loader::loadLutCharlie(const std::string& fileName)
{
    auto node    = m_scene->create<acre::TextureID>(std::hash<std::string>{}(fileName));
    auto texture = node->ptr<acre::TextureID>();

    auto imageR    = createImage(fileName);
    texture->image = {imageR->ptr<acre::ImageID>(), imageR->idx()};

    m_scene->setLutCharlie(node->id<acre::TextureID>());
}

void Loader::loadLutSheenAlbedoScale(const std::string& fileName)
{
    auto node    = m_scene->create<acre::TextureID>(std::hash<std::string>{}(fileName));
    auto texture = node->ptr<acre::TextureID>();

    auto image     = createImage(fileName);
    texture->image = image->id<acre::ImageID>();

    m_scene->setLutSheenAlbedoScale(node->id<acre::TextureID>());
}

void Loader::loadCamera(const std::string& fileName)
{
    auto node   = m_scene->getMainCamera();
    auto camera = node->ptr<acre::CameraID>();
    if (!camera) return;

    std::ifstream file(fileName);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line))
    {
        auto params = splitCameraParameter(line);
        if (params.size() != 2) continue;
        auto key   = params[0];
        auto value = params[1];

        if (key == "ProjectType")
        {
            if (value == "Perspective")
            {
                camera->type = acre::Camera::ProjectType::tPerspective;
            }
            else if (value == "Orthonormal")
            {
                camera->type = acre::Camera::ProjectType::tOrthonormal;
            }
        }
        else if (key == "position")
        {
            auto pos = splitSimpleText(value);
            if (pos.size() != 3) continue;
            camera->position = {std::stof(pos[0]), std::stof(pos[1]), std::stof(pos[2])};
        }
        else if (key == "up")
        {
            auto up = splitSimpleText(value);
            if (up.size() != 3) continue;
            camera->up = {std::stof(up[0]), std::stof(up[1]), std::stof(up[2])};
        }
        else if (key == "target")
        {
            auto target = splitSimpleText(value);
            if (target.size() != 3) continue;
            camera->target = {std::stof(target[0]), std::stof(target[1]), std::stof(target[2])};
        }
        else if (key == "fov")
        {
            auto& projection = std::get<acre::Camera::Perspective>(camera->projection);
            projection.fov   = std::stof(value);
        }
        else if (key == "aspect")
        {
            auto& projection  = std::get<acre::Camera::Perspective>(camera->projection);
            projection.aspect = std::stof(value);
        }
        else if (key == "nearPlane")
        {
            if (camera->type == acre::Camera::ProjectType::tPerspective)
            {
                auto& projection     = std::get<acre::Camera::Perspective>(camera->projection);
                projection.nearPlane = std::stof(value);
            }
            else if (camera->type == acre::Camera::ProjectType::tOrthonormal)
            {
                auto& projection     = std::get<acre::Camera::Orthonormal>(camera->projection);
                projection.nearPlane = std::stof(value);
            }
        }
        else if (key == "farPlane")
        {
            if (camera->type == acre::Camera::ProjectType::tPerspective)
            {
                auto& projection    = std::get<acre::Camera::Perspective>(camera->projection);
                projection.farPlane = std::stof(value);
            }
            else if (camera->type == acre::Camera::ProjectType::tOrthonormal)
            {
                auto& projection    = std::get<acre::Camera::Orthonormal>(camera->projection);
                projection.farPlane = std::stof(value);
            }
        }
        else if (key == "topPlane")
        {
            auto& projection    = std::get<acre::Camera::Orthonormal>(camera->projection);
            projection.topPlane = std::stof(value);
        }
        else if (key == "bottomPlane")
        {
            auto& projection       = std::get<acre::Camera::Orthonormal>(camera->projection);
            projection.bottomPlane = std::stof(value);
        }
        else if (key == "leftPlane")
        {
            auto& projection     = std::get<acre::Camera::Orthonormal>(camera->projection);
            projection.leftPlane = std::stof(value);
        }
        else if (key == "rightPlane")
        {
            auto& projection      = std::get<acre::Camera::Orthonormal>(camera->projection);
            projection.rightPlane = std::stof(value);
        }
    }
}
