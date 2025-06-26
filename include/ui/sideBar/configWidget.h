#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QToolBox>
#include <QListWidget>

class BaseScene;
class ConfigWidget : public QWidget
{
    BaseScene* m_scene;

    QToolBox*    m_toolBox;
    QVBoxLayout* m_layout;

    QListWidget* m_eventList;
    QListWidget* m_actionList;
    QListWidget* m_animationList;

public:
    explicit ConfigWidget(BaseScene* scene, QWidget* parent = nullptr);

    ~ConfigWidget();

private:
    void setupUi();
};
