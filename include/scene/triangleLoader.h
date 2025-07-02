#pragma once
#include <scene/loader.h>
#include <vector>

class TriangleLoader : public Loader
{
public:
    TriangleLoader(SceneMgr* scene);

    virtual ~TriangleLoader() override;

    virtual void loadScene(const std::string& fileName) override;

private:
    void createGeometry();

    void createMaterial();

    void createTransform();

    void createComponent();
};