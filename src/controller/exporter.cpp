#include <controller/exporter.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <fstream>

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

void Exporter::exportCamera(const std::string& fileName)
{
    auto camera = m_scene->getMainCamera();

    std::string cameraInfo;
    cameraInfo += "position: " + std::to_string(camera->position.x) + ", " + std::to_string(camera->position.y) + ", " + std::to_string(camera->position.z) + "\n";
    cameraInfo += "target: " + std::to_string(camera->target.x) + ", " + std::to_string(camera->target.y) + ", " + std::to_string(camera->target.z) + "\n";
    cameraInfo += "up: " + std::to_string(camera->up.x) + ", " + std::to_string(camera->up.y) + ", " + std::to_string(camera->up.z) + "\n";

    if (camera->type == acre::Camera::ProjectType::tPerspective)
    {
        auto& projection = std::get<acre::Camera::Perspective>(camera->projection);
        cameraInfo += "ProjectType: " + std::string("Perspective") + "\n";
        cameraInfo += "fov: " + std::to_string(projection.fov) + "\n";
        cameraInfo += "aspect: " + std::to_string(projection.aspect) + "\n";
        cameraInfo += "nearPlane: " + std::to_string(projection.nearPlane) + "\n";
        cameraInfo += "farPlane: " + std::to_string(projection.farPlane) + "\n";
    }
    if (camera->type == acre::Camera::ProjectType::tOrthonormal)
    {
        auto& projection = std::get<acre::Camera::Orthonormal>(camera->projection);
        cameraInfo += "ProjectType: " + std::string("Orthonormal") + "\n";
        cameraInfo += "nearPlane: " + std::to_string(projection.nearPlane) + "\n";
        cameraInfo += "farPlane: " + std::to_string(projection.farPlane) + "\n";
        cameraInfo += "topPlane: " + std::to_string(projection.topPlane) + "\n";
        cameraInfo += "bottomPlane: " + std::to_string(projection.bottomPlane) + "\n";
        cameraInfo += "leftPlane: " + std::to_string(projection.leftPlane) + "\n";
        cameraInfo += "rightPlane: " + std::to_string(projection.rightPlane) + "\n";
    }

    std::ofstream file(fileName);
    file << cameraInfo;
    file.close();
}
