#pragma once

#include <acre/render/scene/geometry.h>

#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

class SceneMgr;
namespace acre
{
struct Resource;
}
class GeometryWidget : public QWidget
{
    SceneMgr* m_scene;

    acre::Resource* m_geometryR = nullptr;
    acre::Geometry* m_geometry  = nullptr;

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
    explicit GeometryWidget(SceneMgr* scene, QWidget* parent = nullptr);

    ~GeometryWidget();

    void set_geometry(uint32_t uuid);

    void update_properties();

private:
    void _init_ui();
};
