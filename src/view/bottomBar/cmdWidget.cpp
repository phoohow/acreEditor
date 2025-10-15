#include <view/bottom/cmdWidget.h>
#include <controller/cmdController.h>
#include <model/sceneMgr.h>

static QStringList g_cmdList = {
    // single cmd
    "",
    "renderFrame",
    "profiler",
    "pickPixel",
    "saveFrame",
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
    m_cmdController = new CmdController(scene);

    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    m_selector = new QTabWidget(this);
    m_mainLayout->addWidget(m_selector);
    connect(m_selector, &QTabWidget::currentChanged, this, &CmdWidget::onUpdateTab);

    initCmd();

    m_mainLayout->addStretch();
}

void CmdWidget::initCmd()
{
    m_cmdTab      = new QWidget();
    m_cmdLayout   = new QVBoxLayout(m_cmdTab);
    m_cmdLineEdit = new QLineEdit(m_cmdTab);

    m_completer = new QCompleter(g_cmdList, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_cmdLineEdit->setCompleter(m_completer);

    m_cmdLayout->addWidget(m_cmdLineEdit);
    connect(m_cmdLineEdit, &QLineEdit::returnPressed, this, &CmdWidget::onCmdSubmitted);

    m_historyTextEdit = new QTextEdit(m_cmdTab);
    m_historyTextEdit->setReadOnly(true);
    m_cmdLayout->addWidget(m_historyTextEdit);

    m_selector->addTab(m_cmdTab, "Cmd");
    m_cmdLayout->addStretch();
}

void CmdWidget::onUpdateTab()
{
    onCmdTabChanged();
}

void CmdWidget::onCmdTabChanged()
{
    updateCmdInfo();
}

void CmdWidget::updateCmdInfo()
{
}

void CmdWidget::onCmdSubmitted()
{
    QString command = m_cmdLineEdit->text();

    auto  status  = m_cmdController->execute(command.toStdString());
    auto& history = m_cmdController->getHistory();
    m_historyTextEdit->setPlainText(QString::fromStdString(history));

    m_cmdLineEdit->clear();
}

void CmdWidget::setRenderFrameCallBack(std::function<void()> func)
{
    m_cmdController->setRenderFrameCallBack(func);
}

void CmdWidget::setShowProfilerCallBack(std::function<void()> func)
{
    m_cmdController->setShowProfilerCallBack(func);
}

void CmdWidget::setPickPixelCallBack(std::function<void(uint32_t, uint32_t)> func)
{
    m_cmdController->setPickPixelCallBack(func);
}

void CmdWidget::setSaveFrameCallBack(std::function<void()> func)
{
    m_cmdController->setSaveFrameCallBack(func);
}
