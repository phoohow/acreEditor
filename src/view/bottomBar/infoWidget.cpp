#include <view/bottom/infoWidget.h>

#include <model/sceneMgr.h>

InfoWidget::InfoWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_main_layout = new QVBoxLayout(this);
    this->setLayout(m_main_layout);

    m_selector = new QTabWidget(this);
    m_main_layout->addWidget(m_selector);
    connect(m_selector, &QTabWidget::currentChanged, this, &InfoWidget::_on_update_tab);

    _init_state();
    _init_log();

    m_main_layout->addStretch();
    _update_state_info();
}

void InfoWidget::_init_state()
{
    m_state_tab    = new QWidget();
    m_state_layout = new QVBoxLayout(m_state_tab);
    m_state_label  = new QLabel(m_state_tab);
    m_state_layout->addWidget(m_state_label);
    m_selector->addTab(m_state_tab, "State");
    m_state_layout->addStretch();
}

void InfoWidget::_init_log()
{
    m_log_tab      = new QWidget();
    m_log_layout   = new QVBoxLayout(m_log_tab);
    m_log_text_edit = new QTextEdit(m_log_tab);
    m_log_text_edit->setReadOnly(true);
    m_log_layout->addWidget(m_log_text_edit);
    m_selector->addTab(m_log_tab, "Log");
    m_log_layout->addStretch();
}

void InfoWidget::_on_update_tab()
{
    if (m_selector->currentIndex() == 0)
    {
        _on_state_tab_change();
    }
    else if (m_selector->currentIndex() == 1)
    {
        _on_log_tab_change();
    }
}

void InfoWidget::flush_state()
{
    _update_state_info();
}

void InfoWidget::_on_state_tab_change()
{
    _update_state_info();
}

void InfoWidget::_on_log_tab_change()
{
    _update_log_info();
}

void InfoWidget::_update_state_info()
{
    QString stateInfo = "Scene Info: \n";
    stateInfo += "    Entity Count: " + QString::number(m_scene->entity_count()) + "\n";
    stateInfo += "    Geometry Count: " + QString::number(m_scene->geometry_count()) + "\n";
    stateInfo += "    Material Count: " + QString::number(m_scene->material_count()) + "\n";
    stateInfo += "    Texture Count: " + QString::number(m_scene->texture_count()) + "\n";
    stateInfo += "    Image Count: " + QString::number(m_scene->image_count()) + "\n";
    stateInfo += "\nRendering Info: \n";
    // stateInfo += "    AA: " + (m_scene->isAAEnabled() ? "Enabled" : "Disabled") + "\n";
    // stateInfo += "    HDR: " + (m_scene->isHDREnabled() ? "Enabled" : "Disabled") + "\n";
    stateInfo += "\nInteraction Info: \n";
    // stateInfo += "    Pick Info: EntityID: " + QString::number(m_scene->getPickedEntityID());
    // stateInfo += ", GeometryID: " + QString::number(m_scene->getPickedGeometryID());
    // stateInfo += ", MaterialID: " + QString::number(m_scene->getPickedMaterialID()) + "\n";

    m_state_label->setText(stateInfo);
}

void InfoWidget::_update_log_info()
{
    QString logInfo = "";
    // logInfo += m_scene->getLogEntries();
    m_log_text_edit->setText(logInfo);
}

void InfoWidget::show_profiler(const std::string& profiler)
{
    m_selector->setCurrentIndex(1);
    m_log_text_edit->setText(profiler.c_str());
}

void InfoWidget::show_pick_info(const std::string& info)
{
    m_selector->setCurrentIndex(0);
    m_state_label->setText(info.c_str());
}
