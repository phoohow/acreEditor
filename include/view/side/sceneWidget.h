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

    std::function<void()> m_flushFrameFunc;

    QVBoxLayout*    m_layout;
    QTreeWidget*    m_selector;
    QStackedWidget* m_editorStack;

    QWidget* m_cameraWidget;
    QWidget* m_lightWidget;
    QWidget* m_geometryWidget;
    QWidget* m_materialWidget;
    QWidget* m_transformWidget;

    QTreeWidgetItem* m_cameraRoot    = nullptr;
    QTreeWidgetItem* m_lightRoot     = nullptr;
    QTreeWidgetItem* m_geometryRoot  = nullptr;
    QTreeWidgetItem* m_materialRoot  = nullptr;
    QTreeWidgetItem* m_transformRoot = nullptr;

public:
    explicit SceneWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func);

    void onUpdate();

private:
    void initCamera();
    void initLight();
    void initGeometry();
    void initMaterial();
    void initTransform();

    void onTreeItemSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev);

    void updateWidget(QTreeWidgetItem* current, TabWidget tab);
};
