#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QStackedWidget>

class BaseScene;
class SceneWidget;
class ConfigWidget;
class SideBar : public QWidget
{
    BaseScene* m_scene;

    QVBoxLayout*    m_layout;
    QTabWidget*     m_selector;
    QStackedWidget* m_editorStack;

    SceneWidget*  m_sceneWidget;
    ConfigWidget* m_configWidget;

public:
    SideBar(BaseScene* scene, QWidget* parent = nullptr);

    ~SideBar();

    void setFlushFrameCallBack(std::function<void()> func);
};