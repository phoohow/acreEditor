#pragma once

#include <model/sceneMgr.h>

#include <string>

class Loader
{
protected:
    SceneMgr* m_scene = nullptr;

public:
    Loader(SceneMgr* scene);

    virtual ~Loader() {}

    virtual void loadScene(const std::string& fileName) = 0;

    void loadImage(const std::string& fileName);

    void loadHDR(const std::string& fileName);

    void loadLutGGX(const std::string& fileName);

    void loadLutCharlie(const std::string& fileName);

    void loadLutSheenAlbedoScale(const std::string& fileName);

    /**
     * @brief load camera from file
     * @note format example:
     *      ProjectType: perspective
     *      fov: 60
     *      aspect: 1.0
     *      near: 0.1
     *      far: 1000
     */
    void loadCamera(const std::string& fileName);
};
