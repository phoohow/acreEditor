#include <ui/sideBar/configWidget.h>
#include <scene/gltfScene.h>

ConfigWidget::ConfigWidget(BaseScene* scene, QWidget* parent) :
    QWidget(parent), m_scene(scene)
{
    setupUi();
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::setupUi()
{
    // Setup UI elements specific to the config widget
}
