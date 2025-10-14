#include <view/side/sceneWidget.h>
#include <view/side/lightWidget.h>
#include <view/side/cameraWidget.h>
#include <view/side/materialWidget.h>
#include <view/side/geometryWidget.h>
#include <view/side/transformWidget.h>

#include <model/sceneMgr.h>

#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QBrush>
#include <QColor>

#include <stdint.h>
#include <unordered_map>

#define toCameraWidget    static_cast<CameraWidget*>(m_cameraWidget)
#define toLightWidget     static_cast<LightWidget*>(m_lightWidget)
#define toGeometryWidget  static_cast<GeometryWidget*>(m_geometryWidget)
#define toMaterialWidget  static_cast<MaterialWidget*>(m_materialWidget)
#define toTransformWidget static_cast<TransformWidget*>(m_transformWidget)

SceneWidget::SceneWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_layout = new QVBoxLayout(this);

    m_selector = new QTreeWidget(this);
    m_selector->setHeaderHidden(true);
    m_selector->setStyleSheet("QTreeWidget { background-color:rgb(242, 243, 242); border: none; } QTreeWidget::item { height: 24px; }");
    m_layout->addWidget(m_selector);
    m_layout->setStretchFactor(m_selector, 1);

    m_editorStack = new QStackedWidget(this);
    m_layout->addWidget(m_editorStack);
    m_layout->setStretchFactor(m_editorStack, 2);

    this->setLayout(m_layout);
    connect(m_selector, &QTreeWidget::currentItemChanged, this, &SceneWidget::onTreeItemSelected);

    initCamera();
    initLight();
    initGeometry();
    initMaterial();
    initTransform();
}

void SceneWidget::initCamera()
{
    m_cameraWidget = new CameraWidget(this);
    m_editorStack->addWidget(m_cameraWidget);
    m_cameraRoot = new QTreeWidgetItem(m_selector);
    m_cameraRoot->setText(0, "Camera");
    // m_cameraRoot->setExpanded(true);
    m_selector->addTopLevelItem(m_cameraRoot);
}

void SceneWidget::initLight()
{
    m_lightWidget = new LightWidget(m_scene, this);
    m_editorStack->addWidget(m_lightWidget);
    m_lightRoot = new QTreeWidgetItem(m_selector);
    m_lightRoot->setText(0, "Light");
    // m_lightRoot->setExpanded(true);
    m_selector->addTopLevelItem(m_lightRoot);
}

void SceneWidget::initGeometry()
{
    m_geometryWidget = new GeometryWidget(m_scene, this);
    m_editorStack->addWidget(m_geometryWidget);
    m_geometryRoot = new QTreeWidgetItem(m_selector);
    m_geometryRoot->setText(0, "Geometry");
    // m_geometryRoot->setExpanded(true);
    m_selector->addTopLevelItem(m_geometryRoot);
}

void SceneWidget::initMaterial()
{
    m_materialWidget = new MaterialWidget(m_scene, this);
    m_editorStack->addWidget(m_materialWidget);
    m_materialRoot = new QTreeWidgetItem(m_selector);
    m_materialRoot->setText(0, "Material");
    // m_materialRoot->setExpanded(true);
    m_selector->addTopLevelItem(m_materialRoot);
}

void SceneWidget::initTransform()
{
    m_transformWidget = new TransformWidget(m_scene, this);
    m_editorStack->addWidget(m_transformWidget);
    m_transformRoot = new QTreeWidgetItem(m_selector);
    m_transformRoot->setText(0, "Transform");
    // m_transformRoot->setExpanded(true);
    m_selector->addTopLevelItem(m_transformRoot);
}

void SceneWidget::onUpdate()
{
    updateWidget(m_cameraRoot, TabWidget::wCamera);
    updateWidget(m_lightRoot, TabWidget::wLight);
    updateWidget(m_geometryRoot, TabWidget::wGeometry);
    updateWidget(m_materialRoot, TabWidget::wMaterial);
    updateWidget(m_transformRoot, TabWidget::wTransform);
}

void SceneWidget::onTreeItemSelected(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
    static const std::unordered_map<QString, TabWidget> kTabNameToEnum =
        {
            {"Camera", TabWidget::wCamera},
            {"Light", TabWidget::wLight},
            {"Geometry", TabWidget::wGeometry},
            {"Material", TabWidget::wMaterial},
            {"Transform", TabWidget::wTransform},
        };

    if (!current) return;
    QTreeWidgetItem* parent = current->parent();
    const QString    name   = parent ? parent->text(0) : current->text(0);
    const auto       it     = kTabNameToEnum.find(name);
    if (it == kTabNameToEnum.end()) return;

    TabWidget tab = it->second;
    m_editorStack->setCurrentIndex(static_cast<int>(tab));

    if (parent)
    {
        switch (tab)
        {
            case TabWidget::wCamera:
                m_scene->setMainCamera(current->text(0).toInt());
                toCameraWidget->setCamera(m_scene->getMainCamera());
                break;
            case TabWidget::wLight:
            {
                QString selectedText = current->text(0);
                if (selectedText == "Sun")
                {
                    toLightWidget->disableHDR();
                    toLightWidget->enableSun();
                    toLightWidget->updateSunProperties();
                }
                else if (selectedText == "HDR")
                {
                    toLightWidget->disableSun();
                    toLightWidget->enableHDR();
                    toLightWidget->updateHDRProperties();
                }
                else
                {
                    toLightWidget->disableSun();
                    toLightWidget->disableHDR();
                    toLightWidget->setLight(selectedText.toInt());
                    toLightWidget->updateProperties();
                }
                break;
            }
            case TabWidget::wGeometry:
                toGeometryWidget->setGeometry(current->text(0).toInt());
                toGeometryWidget->updateProperties();
                m_scene->highlightGeometry(current->text(0).toInt());
                m_renderFrameFunc();
                break;
            case TabWidget::wMaterial:
                toMaterialWidget->setMaterial(current->text(0).toInt());
                toMaterialWidget->updateProperties();
                m_scene->highlightMaterial(current->text(0).toInt());
                m_renderFrameFunc();
                break;
            case TabWidget::wTransform:
                toTransformWidget->setTransform(current->text(0).toInt());
                toTransformWidget->updateProperties();
                break;
        }
    }
    else
    {
        // updateWidget(current, tab);
        // current->setExpanded(true);

        onUpdate();
    }
}

void SceneWidget::setRenderFrameCallBack(std::function<void()> func)
{
    m_renderFrameFunc = func;
    toMaterialWidget->setRenderFrameCallBack(func);
    toLightWidget->setRenderFrameCallBack(func);
    toCameraWidget->setRenderFrameCallBack(func);
}

void SceneWidget::updateWidget(QTreeWidgetItem* current, TabWidget tab)
{
    switch (tab)
    {
        case TabWidget::wCamera:
        {
            current->takeChildren();
            const auto& cameras = m_scene->getCameraList();
            for (const auto& index : cameras)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(current);
                item->setText(0, QString::number(index.first));
            }
            break;
        }
        case TabWidget::wLight:
        {
            current->takeChildren();
            QTreeWidgetItem* hdrItem = new QTreeWidgetItem(current);
            hdrItem->setText(0, "HDR");
            QTreeWidgetItem* sunItem = new QTreeWidgetItem(current);
            sunItem->setText(0, "Sun");
            const auto& lights = m_scene->getLightList();
            for (const auto& index : lights)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(current);
                item->setText(0, QString::number(index.first));
            }
            break;
        }
        case TabWidget::wGeometry:
        {
            current->takeChildren();
            const auto& geometrys = m_scene->getGeometryList();
            for (const auto& index : geometrys)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(current);
                item->setText(0, QString::number(index.first));
            }
            break;
        }
        case TabWidget::wMaterial:
        {
            current->takeChildren();
            const auto& materials = m_scene->getMaterialList();
            for (const auto& index : materials)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(current);
                item->setText(0, QString::number(index.first));
            }
            break;
        }
        case TabWidget::wTransform:
        {
            current->takeChildren();
            const auto& transforms = m_scene->getTransformList();
            for (const auto& index : transforms)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(current);
                item->setText(0, QString::number(index.first));
            }
            break;
        }
    }
}
