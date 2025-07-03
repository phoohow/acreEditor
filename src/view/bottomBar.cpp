#include <view/bottomBar.h>
#include <view/bottom/cmdWidget.h>
#include <view/bottom/infoWidget.h>

#include <model/sceneMgr.h>

#include <QSplitter>

BottomBar::BottomBar(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    m_cmdWidget  = new CmdWidget(scene, this);
    m_infoWidget = new InfoWidget(scene, this);

    auto hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(m_infoWidget);
    hSplitter->addWidget(m_cmdWidget);
    hSplitter->setSizes({400, 500});

    m_mainLayout->addWidget(hSplitter);

    m_mainLayout->addStretch();
}

void BottomBar::setRenderFrameCallBack(std::function<void()> func)
{
    m_cmdWidget->setRenderFrameCallBack(func);
}

void BottomBar::setSaveFrameCallBack(std::function<void()> func)
{
    m_cmdWidget->setSaveFrameCallBack(func);
}

void BottomBar::setPickPixelCallBack(std::function<void(uint32_t, uint32_t)> func)
{
    m_cmdWidget->setPickPixelCallBack(func);
}

void BottomBar::setShowProfilerCallBack(std::function<void()> func)
{
    m_cmdWidget->setShowProfilerCallBack(func);
}

void BottomBar::flushState()
{
    m_infoWidget->flushState();
}

void BottomBar::showProfiler(const std::string& profiler)
{
    m_infoWidget->showProfiler(profiler);
}

void BottomBar::showPickInfo(const std::string& info)
{
    m_infoWidget->showPickInfo(info);
}
