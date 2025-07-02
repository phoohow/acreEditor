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

    virtual void exportImage(const std::string& fileName, int width, int height, int channel, int pixelByte, void* data);
};
