#include <view/bottomBar.h>
#include <view/bottom/cmdWidget.h>
#include <view/bottom/infoWidget.h>

#include <model/sceneMgr.h>

#include <QSplitter>

BottomBar::BottomBar(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_main_layout = new QVBoxLayout(this);
    this->setLayout(m_main_layout);

    m_cmd_widget  = new CmdWidget(scene, this);
    m_info_widget = new InfoWidget(scene, this);

    auto hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(m_info_widget);
    hSplitter->addWidget(m_cmd_widget);
    hSplitter->setSizes({400, 500});

    m_main_layout->addWidget(hSplitter);

    m_main_layout->addStretch();
}

void BottomBar::set_renderframe_callback(std::function<void()> func)
{
    m_cmd_widget->set_renderframe_callback(func);
}

void BottomBar::set_saveframe_callback(std::function<void()> func)
{
    m_cmd_widget->set_saveframe_callback(func);
}

void BottomBar::set_pickpixel_callback(std::function<void(uint32_t, uint32_t)> func)
{
    m_cmd_widget->set_pickpixel_callback(func);
}

void BottomBar::set_showprofiler_callback(std::function<void()> func)
{
    m_cmd_widget->set_showprofiler_callback(func);
}

void BottomBar::flush_state()
{
    m_info_widget->flush_state();
}

void BottomBar::show_profiler(const std::string& profiler)
{
    m_info_widget->show_profiler(profiler);
}

void BottomBar::show_pick_info(const std::string& info)
{
    m_info_widget->show_pick_info(info);
}
