#pragma once

#include <QWidget>
#include <QVBoxLayout>

#include <functional>

class SceneMgr;
class InfoWidget;
class CmdWidget;
class BottomBar : public QWidget
{
    SceneMgr* m_scene = nullptr;

    QVBoxLayout* m_main_layout;
    InfoWidget*  m_info_widget;
    CmdWidget*   m_cmd_widget;

public:
    explicit BottomBar(SceneMgr* scene, QWidget* parent = nullptr);

    void set_renderframe_callback(std::function<void()> func);

    void set_showprofiler_callback(std::function<void()> func);

    void set_saveframe_callback(std::function<void()> func);

    void set_pickpixel_callback(std::function<void(uint32_t, uint32_t)> func);

    void flush_state();

    void show_profiler(const std::string& profiler);

    void show_pick_info(const std::string& info);
};
