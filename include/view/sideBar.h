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
    QStackedWidget* m_editor_stack;

    SceneWidget*  m_sceneWidget;
    ConfigWidget* m_configWidget;

public:
    SideBar(SceneMgr* scene, QWidget* parent = nullptr);

    ~SideBar();

    void set_renderframe_callback(std::function<void()> func);

private:
    void _on_tab_change(int index);
};
