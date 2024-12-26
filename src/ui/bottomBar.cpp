#include <ui/bottomBar.h>
#include <scene/gltfScene.h>

BottomBar::BottomBar(BaseScene* scene, QWidget* parent) :
    m_scene(scene)
{
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    m_selector = new QTabWidget(this);
    m_mainLayout->addWidget(m_selector);
    connect(m_selector, &QTabWidget::currentChanged, this, &BottomBar::onUpdateTab);

    initState();
    initCmd();
    initLog();

    m_mainLayout->addStretch();
    updateStateInfo();
}

void BottomBar::initState()
{
    m_stateTab    = new QWidget();
    m_stateLayout = new QVBoxLayout(m_stateTab);
    m_stateLabel  = new QLabel(m_stateTab);
    m_stateLayout->addWidget(m_stateLabel);
    m_selector->addTab(m_stateTab, "State");
    m_stateLayout->addStretch();
}

void BottomBar::initCmd()
{
    m_cmdTab      = new QWidget();
    m_cmdLayout   = new QVBoxLayout(m_cmdTab);
    m_cmdLineEdit = new QLineEdit(m_cmdTab);
    m_cmdLayout->addWidget(m_cmdLineEdit);
    connect(m_cmdLineEdit, &QLineEdit::returnPressed, this, &BottomBar::onCmdSubmitted);

    m_cmdResultTextEdit = new QTextEdit(m_cmdTab);
    m_cmdResultTextEdit->setReadOnly(true);
    m_cmdLayout->addWidget(m_cmdResultTextEdit);

    m_selector->addTab(m_cmdTab, "Cmd");
    m_cmdLayout->addStretch();
}

void BottomBar::initLog()
{
    m_logTab      = new QWidget();
    m_logLayout   = new QVBoxLayout(m_logTab);
    m_logTextEdit = new QTextEdit(m_logTab);
    m_logTextEdit->setReadOnly(true);
    m_logLayout->addWidget(m_logTextEdit);
    m_selector->addTab(m_logTab, "Log");
    m_logLayout->addStretch();
}

void BottomBar::onUpdateTab()
{
    if (m_selector->currentIndex() == 0)
    {
        onStateTabChanged();
    }
    else if (m_selector->currentIndex() == 1)
    {
        onCmdTabChanged();
    }
    else if (m_selector->currentIndex() == 2)
    {
        onLogTabChanged();
    }
}

void BottomBar::onStateTabChanged()
{
    updateStateInfo();
}

void BottomBar::onCmdTabChanged()
{
    updateCmdInfo();
}

void BottomBar::onLogTabChanged()
{
    updateLogInfo();
}

void BottomBar::updateStateInfo()
{
    QString stateInfo = "Scene Info: \n";
    stateInfo += "    Entity Count: " + QString::number(m_scene->getEntityCount()) + "\n";
    stateInfo += "    Geometry Count: " + QString::number(m_scene->getGeometryCount()) + "\n";
    stateInfo += "    Material Count: " + QString::number(m_scene->getMaterialCount()) + "\n";
    stateInfo += "    Texture Count: " + QString::number(m_scene->getTextureCount()) + "\n";
    stateInfo += "    Image Count: " + QString::number(m_scene->getImageCount()) + "\n";
    stateInfo += "\nRendering Info: \n";
    // stateInfo += "    AA: " + (m_scene->isAAEnabled() ? "Enabled" : "Disabled") + "\n";
    // stateInfo += "    HDR: " + (m_scene->isHDREnabled() ? "Enabled" : "Disabled") + "\n";
    stateInfo += "\nInteraction Info: \n";
    // stateInfo += "    Pick Info: EntityID: " + QString::number(m_scene->getPickedEntityID());
    // stateInfo += ", GeometryID: " + QString::number(m_scene->getPickedGeometryID());
    // stateInfo += ", MaterialID: " + QString::number(m_scene->getPickedMaterialID()) + "\n";

    m_stateLabel->setText(stateInfo);
}

void BottomBar::updateCmdInfo()
{
}

void BottomBar::updateLogInfo()
{
    QString logInfo = "";
    // logInfo += m_scene->getLogEntries();
    m_logTextEdit->setText(logInfo);
}

void BottomBar::onCmdSubmitted()
{
    QString command = m_cmdLineEdit->text();
    QString result  = processCommand(command);

    m_cmdResultTextEdit->append(result);

    m_cmdLineEdit->clear();
}

QString BottomBar::processCommand(const QString& command)
{
    // TODO
    return ">> " + command;
}