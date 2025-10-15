#pragma once

#include <model/camera.h>
#include <model/sceneMgr.h>

class CameraController
{
    SceneMgr* m_scene  = nullptr;
    Camera*   m_camera = nullptr;

    float m_width  = 0.0f;
    float m_height = 0.0f;

public:
    CameraController(SceneMgr* sceneMgr);

    ~CameraController();

    void reset();
    void resize(float width, float height);

    void move(float x, float y, float z);
    void moveForward();
    void moveBack();

    void rotate(float x, float y, float z);
    void rotateY(float degree);
    void rotateX(float degree);

    void zoom(float distance);

    void topView();
    void bottomView();
    void leftView();
    void rightView();
    void frontView();
    void backView();

private:
    void _init();

    void syncCamera();
};
