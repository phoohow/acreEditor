#pragma once

#include <model/sceneMgr.h>

class Exporter
{
protected:
    SceneMgr* m_scene = nullptr;

public:
    Exporter(SceneMgr* scene);

    virtual ~Exporter();

    virtual void exportScene(const std::string& fileName) {}

    virtual void exportImage(const std::string& fileName, void* data, int width, int height, int channel, int pixelByte);

    virtual void saveFrame(const std::string& fileName, void* data, int width, int height, int channels = 4);
};
