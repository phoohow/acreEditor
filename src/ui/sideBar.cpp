#include <ui/sideBar.h>
#include <ui/sideBar/lightWidget.h>
#include <ui/sideBar/cameraWidget.h>
#include <ui/sideBar/materialWidget.h>
#include <ui/sideBar/geometryWidget.h>
#include <ui/sideBar/transformWidget.h>

#include <scene/gltfScene.h>

#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QBrush>
#include <QColor>
#include <stdint.h>

enum TabWidget : uint8_t
{
    wCamera,
    wLight,
    wGeometry,
    wMaterial,
    wTransform,
};

#define toCameraWidget    static_cast<CameraWidget*>(m_cameraWidget)
#define toLightWidget     static_cast<LightWidget*>(m_lightWidget)
#define toGeometryWidget  static_cast<GeometryWidget*>(m_geometryWidget)
#define toMaterialWidget  static_cast<MaterialWidget*>(m_materialWidget)
#define toTransformWidget static_cast<TransformWidget*>(m_transformWidget)

SideBar::SideBar(BaseScene* scene, QWidget* parent) :
    m_scene(scene)
{
    m_layout = new QVBoxLayout(this);

    m_selector = new QTabWidget(this);
    m_selector->setStyleSheet("QTabWidget { background-color: #f0f0f0; }");
    m_layout->addWidget(m_selector);
    m_layout->setStretchFactor(m_selector, 1);

    m_editorStack = new QStackedWidget(this);
    m_layout->addWidget(m_editorStack);
    m_layout->setStretchFactor(m_editorStack, 2);

    this->setLayout(m_layout);
    connect(m_selector, &QTabWidget::currentChanged, this, &SideBar::onUpdateTab);

    initCamera();
    initLight();
    initGeometry();
    initMaterial();
    initTransform();
}

void SideBar::initCamera()
{
    m_cameraWidget = new CameraWidget(this);
    m_editorStack->addWidget(m_cameraWidget);

    m_cameraList = new QListWidget(this);
    m_cameraList->setStyleSheet("QListWidget { border: none; }");
    m_selector->insertTab(TabWidget::wCamera, m_cameraList, "camera");
    connect(m_cameraList, &QListWidget::itemSelectionChanged, this, &SideBar::onSelectMainCamera);
}

void SideBar::initLight()
{
    m_lightWidget = new LightWidget(m_scene, this);
    m_editorStack->addWidget(m_lightWidget);

    m_lightList = new QListWidget(this);
    m_lightList->setStyleSheet("QListWidget { border: none; }");
    m_selector->insertTab(TabWidget::wLight, m_lightList, "light");
    m_lightList->addItem("HDR");
    m_lightList->addItem("Sun");

    connect(m_lightList, &QListWidget::itemSelectionChanged, this, &SideBar::onSelectLight);
}

void SideBar::initGeometry()
{
    m_geometryWidget = new GeometryWidget(m_scene, this);
    m_editorStack->addWidget(m_geometryWidget);

    m_geometryList = new QListWidget(this);
    m_geometryList->setStyleSheet("QListWidget { border: none; }");
    m_selector->insertTab(TabWidget::wGeometry, m_geometryList, "geometry");
    connect(m_geometryList, &QListWidget::itemSelectionChanged, this, &SideBar::onSelectGeometry);
}

void SideBar::initMaterial()
{
    m_materialWidget = new MaterialWidget(m_scene, this);
    m_editorStack->addWidget(m_materialWidget);

    m_materialList = new QListWidget(this);
    m_materialList->setStyleSheet("QListWidget { border: none; }");
    m_selector->insertTab(TabWidget::wMaterial, m_materialList, "material");
    connect(m_materialList, &QListWidget::itemSelectionChanged, this, &SideBar::onSelectMaterial);
}

void SideBar::initTransform()
{
    m_transformWidget = new TransformWidget(m_scene, this);
    m_editorStack->addWidget(m_transformWidget);

    m_transformList = new QListWidget(this);
    m_transformList->setStyleSheet("QListWidget { border: none; }");
    m_selector->insertTab(TabWidget::wTransform, m_transformList, "transform");
    connect(m_transformList, &QListWidget::itemSelectionChanged, this, &SideBar::onSelectTransform);
}

void SideBar::onUpdateTab()
{
    auto tabIndex = m_selector->currentIndex();
    switch (tabIndex)
    {
        case TabWidget::wCamera:
        {
            auto cameras = m_scene->getCameras();
            for (auto index : cameras)
            {
                auto key         = QString::number(index);
                auto findWidgets = m_cameraList->findItems(key, Qt::MatchExactly);
                if (findWidgets.empty())
                    m_cameraList->addItem(key);
            }
            toCameraWidget->setCamera(m_scene->getCamera());
            toCameraWidget->updateProperties();

            break;
        }
        case TabWidget::wLight:
        {
            auto lights = m_scene->getLights();
            for (auto index : lights)
            {
                auto key         = QString::number(index);
                auto findWidgets = m_lightList->findItems(key, Qt::MatchExactly);
                if (findWidgets.empty())
                    m_lightList->addItem(key);
            }
            break;
        }
        case TabWidget::wGeometry:
        {
            auto geometrys = m_scene->getGeometrys();
            if (geometrys.size() != m_geometryList->count())
            {
                m_geometryList->clear();
            }

            for (auto index : geometrys)
            {
                auto key         = QString::number(index);
                auto findWidgets = m_geometryList->findItems(key, Qt::MatchExactly);
                if (findWidgets.empty())
                    m_geometryList->addItem(key);
            }

            break;
        }
        case TabWidget::wMaterial:
        {
            auto materials = m_scene->getMaterials();
            if (materials.size() != m_materialList->count())
            {
                m_materialList->clear();
            }

            for (auto index : materials)
            {
                auto key         = QString::number(index);
                auto findWidgets = m_materialList->findItems(key, Qt::MatchExactly);
                if (findWidgets.empty())
                    m_materialList->addItem(key);
            }
            break;
        }
        case TabWidget::wTransform:
        {
            auto transforms = m_scene->getTransforms();
            if (transforms.size() != m_transformList->count())
            {
                m_transformList->clear();
            }

            for (auto index : transforms)
            {
                auto key         = QString::number(index);
                auto findWidgets = m_transformList->findItems(key, Qt::MatchExactly);
                if (findWidgets.empty())
                    m_transformList->addItem(key);
            }
            break;
        }
    }

    m_editorStack->setCurrentIndex(tabIndex);
}

void SideBar::onSelectMainCamera()
{
    auto selectedItem = m_cameraList->currentItem();
    if (selectedItem)
    {
        QString selectedText = selectedItem->text();
        m_scene->setMainCamera(selectedText.toInt());
    }

    toCameraWidget->setCamera(m_scene->getCamera());
}

void SideBar::onSelectLight()
{
    auto selectedItem = m_lightList->currentItem();
    if (selectedItem)
    {
        QString selectedText = selectedItem->text();

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
    }
}

void SideBar::onSelectGeometry()
{
    auto selectedItem = m_geometryList->currentItem();
    if (selectedItem)
    {
        QString selectedText = selectedItem->text();

        toGeometryWidget->setGeometry(selectedText.toInt());
        toGeometryWidget->updateProperties();

        m_scene->setHighlightGeometry(selectedText.toInt());
        m_flushFrame();
    }
}

void SideBar::onSelectMaterial()
{
    auto selectedItem = m_materialList->currentItem();
    if (selectedItem)
    {
        QString selectedText = selectedItem->text();

        toMaterialWidget->setMaterial(selectedText.toInt());
        toMaterialWidget->updateProperties();

        m_scene->setHighlightMaterial(selectedText.toInt());
        m_flushFrame();
    }
}

void SideBar::onSelectTransform()
{
    auto selectedItem = m_transformList->currentItem();
    if (selectedItem)
    {
        QString selectedText = selectedItem->text();

        toTransformWidget->setTransform(selectedText.toInt());
        toTransformWidget->updateProperties();
    }
}

void SideBar::setFlushFrameCallBack(std::function<void()> func)
{
    m_flushFrame = func;
    toMaterialWidget->setFlushFrameCallBack(func);
    toLightWidget->setFlushFrameCallBack(func);
    toCameraWidget->setFlushFrameCallBack(func);
}
