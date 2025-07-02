#pragma once

#include <model/camera.h>

class CameraController
{
public:
    CameraController();
    ~CameraController();

    void move(float x, float y, float z);
    void rotate(float x, float y, float z);
    void zoom(float distance);
    void reset();
};