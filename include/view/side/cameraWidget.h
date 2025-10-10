#pragma once

#include <acre/render/scene/camera.h>

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <functional>

namespace acre
{
struct Resource;
}

class CameraWidget : public QWidget
{
    acre::Resource* m_cameraR = nullptr;
    acre::Camera*   m_camera  = nullptr;

    std::function<void()> m_renderFrameFunc;

    QVBoxLayout* m_layout;
    QLabel*      m_label_eye;
    QLineEdit*   m_lineEdit_eye_x;
    QLineEdit*   m_lineEdit_eye_y;
    QLineEdit*   m_lineEdit_eye_z;
    QLabel*      m_label_target;
    QLineEdit*   m_lineEdit_target_x;
    QLineEdit*   m_lineEdit_target_y;
    QLineEdit*   m_lineEdit_target_z;
    QLabel*      m_label_fov;
    QLineEdit*   m_lineEdit_fov;
    QLabel*      m_label_near;
    QLineEdit*   m_lineEdit_near;
    QLabel*      m_label_far;
    QLineEdit*   m_lineEdit_far;

public:
    explicit CameraWidget(QWidget* parent = nullptr);

    ~CameraWidget();

    void setRenderFrameCallBack(std::function<void()> func) { m_renderFrameFunc = func; }

    void setCamera(acre::Resource* camera);

    void updateProperties();

private:
    void initUI();

    void onUpdateEye();
    void onUpdateTarget();
    void onUpdateFov();
    void onUpdateNear();
    void onUpdateFar();
};
