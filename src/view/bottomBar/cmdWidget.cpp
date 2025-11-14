#include <view/bottom/cmdWidget.h>
#include <controller/cmdController.h>
#include <model/sceneMgr.h>

static QStringList g_cmdList = {
    // single cmd
    "",
    "render_frame",
    "profiler",
    "pick_pixel",
    "save_frame",
    "exit",

    // multi cmd
    "clear history",
    "clear hdr",
    "clear scene",
    "clear sunlight",
    "reset view",
    "highlight entity",
    "highlight geometry",
    "highlight material",
    "unhighlight entity",
    "unhighlight geometry",
    "unhighlight material",
    "remove entity",
    "remove geometry",
    "remove material",
    "move entity",
    "move geometry",
    "move camera",
    "rotate entity",
    "rotate geometry",
    "rotate camera",
    "active entity",
    "reset_alive entity",
    "load image",
    "load scene",
};

CmdWidget::CmdWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_cmd_ctrlr = new CmdController(scene);

    m_main_layout = new QVBoxLayout(this);
    this->setLayout(m_main_layout);

    m_selector = new QTabWidget(this);
    m_main_layout->addWidget(m_selector);
    connect(m_selector, &QTabWidget::currentChanged, this, &CmdWidget::_on_update_tab);

    _init_cmd();

    m_main_layout->addStretch();
}

void CmdWidget::_init_cmd()
{
    m_cmd_tab      = new QWidget();
    m_cmd_layout   = new QVBoxLayout(m_cmd_tab);
    m_cmd_line_edit = new QLineEdit(m_cmd_tab);

    m_completer = new QCompleter(g_cmdList, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_cmd_line_edit->setCompleter(m_completer);

    m_cmd_layout->addWidget(m_cmd_line_edit);
    connect(m_cmd_line_edit, &QLineEdit::returnPressed, this, &CmdWidget::_on_cmd_submit);

    m_history_text_edit = new QTextEdit(m_cmd_tab);
    m_history_text_edit->setReadOnly(true);
    m_cmd_layout->addWidget(m_history_text_edit);

    m_selector->addTab(m_cmd_tab, "Cmd");
    m_cmd_layout->addStretch();
}

void CmdWidget::_on_update_tab()
{
    _on_cmd_tab_change();
}

void CmdWidget::_on_cmd_tab_change()
{
    _update_cmd_info();
}

void CmdWidget::_update_cmd_info()
{
}

void CmdWidget::_on_cmd_submit()
{
    QString command = m_cmd_line_edit->text();

    auto  status  = m_cmd_ctrlr->execute(command.toStdString());
    auto& history = m_cmd_ctrlr->getHistory();
    m_history_text_edit->setPlainText(QString::fromStdString(history));

    m_cmd_line_edit->clear();
}

void CmdWidget::set_renderframe_callback(std::function<void()> func)
{
    m_cmd_ctrlr->set_renderframe_callback(func);
}

void CmdWidget::set_showprofiler_callback(std::function<void()> func)
{
    m_cmd_ctrlr->set_showprofiler_callback(func);
}

void CmdWidget::set_pickpixel_callback(std::function<void(uint32_t, uint32_t)> func)
{
    m_cmd_ctrlr->set_pickpixel_callback(func);
}

void CmdWidget::set_saveframe_callback(std::function<void()> func)
{
    m_cmd_ctrlr->set_saveframe_callback(func);
}
