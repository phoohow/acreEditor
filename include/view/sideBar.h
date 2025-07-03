#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QStackedWidget>

class SceneMgr;
class SceneWidget;
class ConfigWidget;
class SideBar : public QWidget
{
    SceneMgr* m_scene;

    QVBoxLayout*    m_layout;
    QTabWidget*     m_selector;
    QStackedWidget* m_editorStack;

    SceneWidget*  m_sceneWidget;
    ConfigWidget* m_configWidget;

public:
    SideBar(SceneMgr* scene, QWidget* parent = nullptr);

    ~SideBar();

    void setRenderFrameCallBack(std::function<void()> func);

private:
    void onTabChanged(int index);
};
