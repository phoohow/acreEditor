#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

#include <functional>

class SceneMgr;
class InfoWidget : public QWidget
{
    SceneMgr* m_scene = nullptr;

    QTabWidget* m_selector;
    QLabel*     m_state_label;
    QTextEdit*  m_log_text_edit;

    QVBoxLayout* m_main_layout;
    QVBoxLayout* m_state_layout;
    QWidget*     m_state_tab;
    QVBoxLayout* m_log_layout;
    QWidget*     m_log_tab;

public:
    explicit InfoWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void flush_state();

    void show_profiler(const std::string& profiler);

    void show_pick_info(const std::string& info);

private:
    void _init_state();
    void _init_log();

    void _on_update_tab();

    void _on_state_tab_change();
    void _on_log_tab_change();
    void _update_state_info();
    void _update_log_info();
};
