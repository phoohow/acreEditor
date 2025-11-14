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

    std::function<void()> m_renderframe_func;

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

    void set_renderframe_callback(std::function<void()> func) { m_renderframe_func = func; }

    void set_camera(acre::Resource* camera);

    void update_properties();

private:
    void _init_ui();

    void _on_update_eye();
    void _on_update_target();
    void _on_update_fov();
    void _on_update_near();
    void _on_update_far();
};
