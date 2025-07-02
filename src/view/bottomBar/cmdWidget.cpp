#include <view/bottom/cmdWidget.h>

#include <model/sceneMgr.h>

CmdWidget::CmdWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
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

    processCommand(command);
}

void CmdWidget::processCommand(QString& command)
{
    if (command == "clear")
    {
        m_cmdLineEdit->clear();
        m_historyTextEdit->clear();
        return;
    }
    else if (command == "")
    {
        m_cmdLineEdit->clear();
        return;
    }
    else
    {
        command = "UnSupported Command: " + command;
    }

    QString result = ">> " + command;
    m_historyTextEdit->append(result);

    m_cmdLineEdit->clear();
}