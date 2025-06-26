#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QListWidget>
#include <QVBoxLayout>

class BaseScene;
class SceneWidget : public QWidget
{
    BaseScene* m_scene = nullptr;

    std::function<void()> m_flushFrame;

    QVBoxLayout*    m_layout;
    QTabWidget*     m_selector;
    QStackedWidget* m_editorStack;

    QListWidget* m_cameraList;
    QListWidget* m_lightList;
    QListWidget* m_geometryList;
    QListWidget* m_materialList;
    QListWidget* m_transformList;

    QWidget* m_cameraWidget;
    QWidget* m_lightWidget;
    QWidget* m_geometryWidget;
    QWidget* m_materialWidget;
    QWidget* m_transformWidget;

public:
    explicit SceneWidget(BaseScene* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func);

private:
    void initCamera();
    void initLight();
    void initGeometry();
    void initMaterial();
    void initTransform();

    void onUpdateTab();

    void onSelectMainCamera();
    void onSelectLight();
    void onSelectGeometry();
    void onSelectMaterial();
    void onSelectTransform();
};
