#include <view/side/configWidget.h>

#include <model/sceneMgr.h>

ConfigWidget::ConfigWidget(SceneMgr* scene, QWidget* parent) :
    QWidget(parent), m_scene(scene)
{
    m_toolBox       = new QToolBox(this);
    m_eventList     = new QListWidget(this);
    m_actionList    = new QListWidget(this);
    m_animationList = new QListWidget(this);

    m_toolBox->addItem(m_eventList, "Events");
    m_toolBox->addItem(m_actionList, "Actions");
    m_toolBox->addItem(m_animationList, "Animations");

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_toolBox);
    setLayout(m_layout);

    _setup_ui();
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::_setup_ui()
{
    m_eventList->addItem("Event  A");
    m_eventList->addItem("Event  B");
    m_eventList->addItem("Event  C");

    m_actionList->addItem("Action  A");
    m_actionList->addItem("Action  B");
    m_actionList->addItem("Action  C");

    m_animationList->addItem("Animation  A");
    m_animationList->addItem("Animation  B");
    m_animationList->addItem("Animation  C");
}
