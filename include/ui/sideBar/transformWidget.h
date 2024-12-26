#pragma once

#include <Render/scene/transform.h>
#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

class BaseScene;
class TransformWidget : public QWidget
{
    BaseScene*        m_scene;
    rdr::TransformPtr m_transform;
    rdr::TransformID  m_transformID;

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
    explicit TransformWidget(BaseScene* scene, QWidget* parent = nullptr);

    ~TransformWidget();

    void setTransform(rdr::TransformID id);

    void updateProperties();

private:
    void initUI();

    void onUpdateTranslation();
    void onUpdateRotation();
    void onUpdateScale();
};
