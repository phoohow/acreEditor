#pragma once

#include <acre/render/scene/transform.h>
#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

class SceneMgr;
class TransformWidget : public QWidget
{
    SceneMgr*        m_scene;
    acre::TransformPtr m_transform;
    acre::TransformID  m_transformID;

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

    void setTransform(acre::TransformID id);

    void updateProperties();

private:
    void initUI();

    void onUpdateTranslation();
    void onUpdateRotation();
    void onUpdateScale();
};
