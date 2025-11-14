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
#include <QPushButton>

#include <stdint.h>
#include <unordered_map>

#define toCameraWidget    static_cast<CameraWidget*>(m_camera_widget)
#define toLightWidget     static_cast<LightWidget*>(m_light_widget)
#define toGeometryWidget  static_cast<GeometryWidget*>(m_geometry_widget)
#define toMaterialWidget  static_cast<MaterialWidget*>(m_material_widget)
#define toTransformWidget static_cast<TransformWidget*>(m_transform_widget)

SceneWidget::SceneWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene)
{
    m_layout = new QVBoxLayout(this);

    m_selector = new QTreeWidget(this);
    m_selector->setHeaderHidden(true);
    m_selector->setStyleSheet("QTreeWidget { background-color:rgb(242, 243, 242); border: none; } QTreeWidget::item { height: 24px; }");
    m_layout->addWidget(m_selector);
    m_layout->setStretchFactor(m_selector, 1);

    m_editor_stack = new QStackedWidget(this);
    m_layout->addWidget(m_editor_stack);
    m_layout->setStretchFactor(m_editor_stack, 2);

    this->setLayout(m_layout);
    connect(m_selector, &QTreeWidget::currentItemChanged, this, &SceneWidget::_on_treeitem_selected);

    _init_camera();
    _init_light();
    _init_geometry();
    _init_material();
    _init_transform();
}

void SceneWidget::_init_camera()
{
    m_camera_widget = new CameraWidget(this);
    m_editor_stack->addWidget(m_camera_widget);
    m_camera_root = new QTreeWidgetItem(m_selector);
    m_camera_root->setText(0, "Camera");
    // m_camera_root->setExpanded(true);
    m_selector->addTopLevelItem(m_camera_root);
}

void SceneWidget::_init_light()
{
    m_light_widget = new LightWidget(m_scene, this);
    m_editor_stack->addWidget(m_light_widget);
    m_light_root = new QTreeWidgetItem(m_selector);
    m_light_root->setText(0, "Light");
    // m_light_root->setExpanded(true);
    m_selector->addTopLevelItem(m_light_root);
}

void SceneWidget::_init_geometry()
{
    m_geometry_widget = new GeometryWidget(m_scene, this);
    m_editor_stack->addWidget(m_geometry_widget);
    m_geometry_root = new QTreeWidgetItem(m_selector);
    m_geometry_root->setText(0, "Geometry");
    // m_geometry_root->setExpanded(true);
    m_selector->addTopLevelItem(m_geometry_root);
}

void SceneWidget::_init_material()
{
    m_material_widget = new MaterialWidget(m_scene, this);
    m_editor_stack->addWidget(m_material_widget);
    m_material_root = new QTreeWidgetItem(m_selector);
    m_material_root->setText(0, "Material");
    // m_material_root->setExpanded(true);
    m_selector->addTopLevelItem(m_material_root);
}

void SceneWidget::_init_transform()
{
    m_transform_widget = new TransformWidget(m_scene, this);
    m_editor_stack->addWidget(m_transform_widget);
    m_transform_root = new QTreeWidgetItem(m_selector);
    m_transform_root->setText(0, "Transform");
    // m_transform_root->setExpanded(true);
    m_selector->addTopLevelItem(m_transform_root);
}

void SceneWidget::on_update()
{
    _update_widget(m_camera_root, TabWidget::wCamera);
    _update_widget(m_light_root, TabWidget::wLight);
    _update_widget(m_geometry_root, TabWidget::wGeometry);
    _update_widget(m_material_root, TabWidget::wMaterial);
    _update_widget(m_transform_root, TabWidget::wTransform);
}

void SceneWidget::_on_treeitem_selected(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
    static const std::unordered_map<QString, TabWidget> kTabNameToEnum = {
        {"Camera", TabWidget::wCamera},
        {"Light", TabWidget::wLight},
        {"Geometry", TabWidget::wGeometry},
        {"Material", TabWidget::wMaterial},
        {"Transform", TabWidget::wTransform},
    };

    if (!current) return;
    QTreeWidgetItem* parent = current->parent();
    const QString    name   = parent ? parent->text(0) : current->text(0);

    const auto it = kTabNameToEnum.find(name);
    if (it == kTabNameToEnum.end()) return;

    TabWidget tab = it->second;
    m_editor_stack->setCurrentIndex(static_cast<int>(tab));

    if (parent)
    {
        switch (tab)
        {
            case TabWidget::wCamera:
                m_scene->set_main_camera(current->text(0).toInt());
                toCameraWidget->set_camera(m_scene->main_camera());
                break;
            case TabWidget::wLight:
            {
                QString selectedText = current->text(0);
                if (selectedText == "Sun")
                {
                    toLightWidget->disable_hdr();
                    toLightWidget->enable_sun();
                    toLightWidget->update_sun_properties();
                }
                else if (selectedText == "HDR")
                {
                    toLightWidget->disable_sun();
                    toLightWidget->enable_hdr();
                    toLightWidget->update_hdr_properties();
                }
                else
                {
                    toLightWidget->disable_sun();
                    toLightWidget->disable_hdr();
                    toLightWidget->set_light(selectedText.toInt());
                    toLightWidget->update_properties();
                }
                break;
            }
            case TabWidget::wGeometry:
                toGeometryWidget->set_geometry(current->text(0).toInt());
                toGeometryWidget->update_properties();
                m_scene->highlight_geometry(current->text(0).toInt());
                m_renderframe_func();
                break;
            case TabWidget::wMaterial:
                toMaterialWidget->set_material(current->text(0).toInt());
                toMaterialWidget->update_properties();
                m_scene->highlight_material(current->text(0).toInt());
                m_renderframe_func();
                break;
            case TabWidget::wTransform:
                toTransformWidget->set_transform(current->text(0).toInt());
                toTransformWidget->update_properties();
                break;
        }
    }
    else
    {
        // _update_widget(current, tab);
        // current->setExpanded(true);
        on_update();
    }
}

void SceneWidget::set_renderframe_callback(std::function<void()> func)
{
    m_renderframe_func = func;
    toMaterialWidget->set_renderframe_callback(func);
    toLightWidget->set_renderframe_callback(func);
    toCameraWidget->set_renderframe_callback(func);
}

void SceneWidget::_update_widget(QTreeWidgetItem* current, TabWidget tab)
{
    switch (tab)
    {
        case TabWidget::wCamera:
        {
            current->takeChildren();
            const auto& cameras = m_scene->camera_list();
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
            const auto& lights = m_scene->light_list();
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
            const auto& geometrys = m_scene->geometry_list();
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
            const auto& materials = m_scene->material_list();
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
            const auto& transforms = m_scene->transform_list();
            for (const auto& index : transforms)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(current);
                item->setText(0, QString::number(index.first));
            }
            break;
        }
    }
}
