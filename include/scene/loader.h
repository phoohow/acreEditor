#pragma once

#include "scene/sceneMgr.h"

#include <string>

class Loader
{
protected:
    SceneMgr* m_scene = nullptr;

public:
    Loader(SceneMgr* scene);

    virtual ~Loader() {}

    virtual void loadScene(const std::string& fileName) = 0;

    virtual void loadHDR(const std::string& fileName);

    virtual void loadImage(const std::string& fileName);
};
