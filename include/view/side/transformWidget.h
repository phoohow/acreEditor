#pragma once

#include <acre/render/scene/transform.h>

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class SceneMgr;
namespace acre
{
struct Resource;
}
class TransformWidget : public QWidget
{
    SceneMgr*        m_scene;
    acre::Resource*  m_transformR;
    acre::Transform* m_transform;

    QVBoxLayout* m_layout;
    QLabel*      m_label_translation;
    QLabel*      m_label_rotation;
    QLabel*      m_label_scale;
    QLineEdit*   m_lineEdit_translation_x;
    QLineEdit*   m_lineEdit_translation_y;
    QLineEdit*   m_lineEdit_translation_z;
    QLineEdit*   m_lineEdit_rotation_x;
    QLineEdit*   m_lineEdit_rotation_y;
    QLineEdit*   m_lineEdit_rotation_z;
    QLineEdit*   m_lineEdit_scale_x;
    QLineEdit*   m_lineEdit_scale_y;
    QLineEdit*   m_lineEdit_scale_z;

public:
    explicit TransformWidget(SceneMgr* scene, QWidget* parent = nullptr);

    ~TransformWidget();

    void setTransform(uint32_t uuid);

    void updateProperties();

private:
    void initUI();

    void onUpdateTranslation();
    void onUpdateRotation();
    void onUpdateScale();
};
