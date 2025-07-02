#include <controller/exporter.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

Exporter::Exporter(SceneMgr* scene) :
    m_scene(scene)
{
}

Exporter::~Exporter()
{
}

void Exporter::exportImage(const std::string& fileName, void* data, int width, int height, int channels, int pixelByte)
{
    if (fileName.find(".bmp") != std::string::npos)
    {
        stbi_write_bmp(fileName.c_str(), width, height, channels, data);
    }
    else if (fileName.find(".png") != std::string::npos)
    {
        stbi_write_png(fileName.c_str(), width, height, channels, data, width * channels * pixelByte);
    }
}

void Exporter::saveFrame(const std::string& fileName, void* data, int width, int height, int channels)
{
    stbi_write_bmp(fileName.c_str(), width, height, channels, data);
}
