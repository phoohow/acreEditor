#pragma once

#include <Render/scene/camera.h>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <functional>

class CameraWidget : public QWidget
{
    rdr::CameraPtr m_camera = nullptr;

    std::function<void()> m_flushFrame;

    QVBoxLayout* m_layout;
    QLabel*      m_label_eye;
    QLineEdit*   m_lineEdit_eye_x;
    QLineEdit*   m_lineEdit_eye_y;
    QLineEdit*   m_lineEdit_eye_z;
    QLabel*      m_label_fov;
    QLineEdit*   m_lineEdit_fov;
    QLabel*      m_label_near;
    QLineEdit*   m_lineEdit_near;
    QLabel*      m_label_far;
    QLineEdit*   m_lineEdit_far;

public:
    explicit CameraWidget(QWidget* parent = nullptr);

    ~CameraWidget();

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

    void setCamera(rdr::CameraPtr camera) { m_camera = camera; }

    void updateProperties();

private:
    void initUI();

    void onUpdateEye();
    void onUpdateFov();
    void onUpdateNear();
    void onUpdateFar();
};