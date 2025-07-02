#include <ui/sideBar.h>
#include <ui/sideBar/sceneWidget.h>
#include <ui/sideBar/configWidget.h>
#include <scene/sceneMgr.h>

SideBar::SideBar(SceneMgr* scene, QWidget* parent) :
    QWidget(parent), m_scene(scene)
{
    m_scene        = scene;
    m_sceneWidget  = new SceneWidget(scene, this);
    m_configWidget = new ConfigWidget(scene, this);

    m_editorStack = new QStackedWidget(this);
    m_editorStack->addWidget(m_sceneWidget);
    m_editorStack->addWidget(m_configWidget);

    m_selector = new QTabWidget(this);
    m_selector->addTab(m_sceneWidget, tr("Scene"));
    m_selector->addTab(m_configWidget, tr("Config"));
    connect(m_selector, &QTabWidget::currentChanged, this, &SideBar::onTabChanged);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_selector);
    m_layout->addWidget(m_editorStack);
    setLayout(m_layout);
}

SideBar::~SideBar()
{
    delete m_sceneWidget;
    delete m_configWidget;
    delete m_layout;
}

void SideBar::setFlushFrameCallBack(std::function<void()> func)
{
    m_sceneWidget->setFlushFrameCallBack(func);
}

void SideBar::onTabChanged(int index)
{
    if (index == 0)
    {
        m_sceneWidget->onUpdate();
    }
}