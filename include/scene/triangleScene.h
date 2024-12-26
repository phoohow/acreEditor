#pragma once
#include <scene/baseScene.h>
#include <vector>

class TriangleScene : public BaseScene
{

public:
    TriangleScene(acre::Scene* scene);

    virtual ~TriangleScene() override;

    virtual void createScene() override;

    void registerResource();

    void destory();

private:
    void createEntity();

    void createGeometry();

    void createMaterial();

    void createTransform();

    void createCamera();
};