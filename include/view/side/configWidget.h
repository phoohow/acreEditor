#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QToolBox>
#include <QListWidget>

class SceneMgr;
class ConfigWidget : public QWidget
{
    SceneMgr* m_scene;

    QToolBox*    m_toolBox;
    QVBoxLayout* m_layout;

    QListWidget* m_eventList;
    QListWidget* m_actionList;
    QListWidget* m_animationList;

public:
    explicit ConfigWidget(SceneMgr* scene, QWidget* parent = nullptr);

    ~ConfigWidget();

private:
    void _setup_ui();
};
