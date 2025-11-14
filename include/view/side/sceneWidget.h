#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <functional>

class SceneMgr;
class SceneWidget : public QWidget
{
    enum TabWidget : uint8_t
    {
        wCamera,
        wLight,
        wGeometry,
        wMaterial,
        wTransform,
    };

    SceneMgr* m_scene = nullptr;

    std::function<void()> m_renderframe_func;

    QVBoxLayout*    m_layout;
    QTreeWidget*    m_selector;
    QStackedWidget* m_editor_stack;

    QWidget* m_camera_widget;
    QWidget* m_light_widget;
    QWidget* m_geometry_widget;
    QWidget* m_material_widget;
    QWidget* m_transform_widget;

    QTreeWidgetItem* m_camera_root    = nullptr;
    QTreeWidgetItem* m_light_root     = nullptr;
    QTreeWidgetItem* m_geometry_root  = nullptr;
    QTreeWidgetItem* m_material_root  = nullptr;
    QTreeWidgetItem* m_transform_root = nullptr;

public:
    explicit SceneWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void set_renderframe_callback(std::function<void()> func);

    void on_update();

private:
    void _init_camera();
    void _init_light();
    void _init_geometry();
    void _init_material();
    void _init_transform();

    void _on_treeitem_selected(QTreeWidgetItem* current, QTreeWidgetItem* prev);

    void _update_widget(QTreeWidgetItem* current, TabWidget tab);
};
