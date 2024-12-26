#pragma once

#include <acre/render/scene/geometry.h>
#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

class BaseScene;
class GeometryWidget : public QWidget
{
    BaseScene*       m_scene;
    acre::GeometryID  m_geometryID = -1;
    acre::GeometryPtr m_geometry;

    QVBoxLayout* m_layout;
    QCheckBox*   m_checkbox_index;
    QCheckBox*   m_checkbox_position;
    QCheckBox*   m_checkbox_uv;
    QCheckBox*   m_checkbox_normal;
    QCheckBox*   m_checkbox_tangent;
    QCheckBox*   m_checkbox_color;
    QLabel*      m_label_indexCount;
    QLabel*      m_label_vertexCount;
    QLineEdit*   m_lineEdit_indexCount;
    QLineEdit*   m_lineEdit_vertexCount;

public:
    explicit GeometryWidget(BaseScene* scene, QWidget* parent = nullptr);

    ~GeometryWidget();

    void setGeometry(acre::GeometryID id);

    void updateProperties();

private:
    void initUI();
};
