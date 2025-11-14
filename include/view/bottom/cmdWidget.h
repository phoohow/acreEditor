#pragma once

#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QCompleter>

#include <functional>

class SceneMgr;
class CmdController;
class CmdWidget : public QWidget
{
    SceneMgr*      m_scene     = nullptr;
    CmdController* m_cmd_ctrlr = nullptr;

    QTabWidget* m_selector;
    QLineEdit*  m_cmd_line_edit;
    QCompleter* m_completer;
    QTextEdit*  m_history_text_edit;

    QVBoxLayout* m_main_layout;
    QVBoxLayout* m_cmd_layout;
    QWidget*     m_cmd_tab;

public:
    explicit CmdWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void set_renderframe_callback(std::function<void()> func);

    void set_showprofiler_callback(std::function<void()> func);

    void set_pickpixel_callback(std::function<void(uint32_t, uint32_t)> func);

    void set_saveframe_callback(std::function<void()> func);

private:
    void _init_cmd();

    void _on_update_tab();

    void _on_cmd_tab_change();
    void _update_cmd_info();
    void _on_cmd_submit();

    void _process_cmd(QString& command);
};
