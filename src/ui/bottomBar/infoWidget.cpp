#include <ui/bottomBar/infoWidget.h>
#include <scene/sceneMgr.h>

InfoWidget::InfoWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    m_selector = new QTabWidget(this);
    m_mainLayout->addWidget(m_selector);
    connect(m_selector, &QTabWidget::currentChanged, this, &InfoWidget::onUpdateTab);

    initState();
    initLog();

    m_mainLayout->addStretch();
    updateStateInfo();
}

void InfoWidget::initState()
{
    m_stateTab    = new QWidget();
    m_stateLayout = new QVBoxLayout(m_stateTab);
    m_stateLabel  = new QLabel(m_stateTab);
    m_stateLayout->addWidget(m_stateLabel);
    m_selector->addTab(m_stateTab, "State");
    m_stateLayout->addStretch();
}

void InfoWidget::initLog()
{
    m_logTab      = new QWidget();
    m_logLayout   = new QVBoxLayout(m_logTab);
    m_logTextEdit = new QTextEdit(m_logTab);
    m_logTextEdit->setReadOnly(true);
    m_logLayout->addWidget(m_logTextEdit);
    m_selector->addTab(m_logTab, "Log");
    m_logLayout->addStretch();
}

void InfoWidget::onUpdateTab()
{
    if (m_selector->currentIndex() == 0)
    {
        onStateTabChanged();
    }
    else if (m_selector->currentIndex() == 1)
    {
        onLogTabChanged();
    }
}

void InfoWidget::flushState()
{
    updateStateInfo();
}

void InfoWidget::onStateTabChanged()
{
    updateStateInfo();
}

void InfoWidget::onLogTabChanged()
{
    updateLogInfo();
}

void InfoWidget::updateStateInfo()
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

void InfoWidget::updateLogInfo()
{
    QString logInfo = "";
    // logInfo += m_scene->getLogEntries();
    m_logTextEdit->setText(logInfo);
}
