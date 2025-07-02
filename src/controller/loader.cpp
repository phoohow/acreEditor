#include <controller/loader.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>

Loader::Loader(SceneMgr* scene) :
    m_scene(scene) {}

void Loader::loadHDR(const std::string& fileName)
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

    // TODO:
    // clearHDR();
    // m_hdrImages.emplace_back(imageID);
    // m_hdrTextures.emplace_back(textureID);
}

void Loader::loadImage(const std::string& fileName)
{
}
