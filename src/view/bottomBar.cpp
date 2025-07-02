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

void BottomBar::setFlushFrameCallBack(std::function<void()> func)
{
    m_flushFrame = func;
}

void BottomBar::flushState()
{
    m_infoWidget->flushState();
}
