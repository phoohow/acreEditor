#include <controller/exporter.h>

Exporter::Exporter(SceneMgr* scene) :
    m_scene(scene)
{
}

Exporter::~Exporter()
{
}

void Exporter::exportImage(const std::string& fileName, int width, int height, int channel, int pixelByte, void* data)
{
}
