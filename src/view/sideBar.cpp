#include <view/sideBar.h>
#include <view/side/sceneWidget.h>
#include <view/side/configWidget.h>

#include <model/sceneMgr.h>

SideBar::SideBar(SceneMgr* scene, QWidget* parent) :
    QWidget(parent), m_scene(scene)
{
    m_scene        = scene;
    m_sceneWidget  = new SceneWidget(scene, this);
    m_configWidget = new ConfigWidget(scene, this);

    m_editor_stack = new QStackedWidget(this);
    m_editor_stack->addWidget(m_sceneWidget);
    m_editor_stack->addWidget(m_configWidget);

    m_selector = new QTabWidget(this);
    m_selector->addTab(m_sceneWidget, tr("Scene"));
    m_selector->addTab(m_configWidget, tr("Config"));
    connect(m_selector, &QTabWidget::currentChanged, this, &SideBar::_on_tab_change);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_selector);
    m_layout->addWidget(m_editor_stack);
    setLayout(m_layout);
}

SideBar::~SideBar()
{
    delete m_sceneWidget;
    delete m_configWidget;
    delete m_layout;
}

void SideBar::set_renderframe_callback(std::function<void()> func)
{
    m_sceneWidget->set_renderframe_callback(func);
}

void SideBar::_on_tab_change(int index)
{
    if (index == 0)
    {
        m_sceneWidget->on_update();
    }
}