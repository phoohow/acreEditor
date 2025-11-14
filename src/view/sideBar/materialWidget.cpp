#include <view/side/materialWidget.h>

#include <model/sceneMgr.h>

#include <QDoubleValidator>
#include <QIntValidator>

MaterialWidget::MaterialWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene),
    QWidget(parent)
{
    _init_ui();
}

MaterialWidget::~MaterialWidget()
{
    // Clean up if necessary
}

void MaterialWidget::_init_ui()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);

    m_layout_type = new QHBoxLayout();
    m_label_type  = new QLabel("type:");
    m_label_type->setFixedWidth(90);
    m_comboBox_type = new QComboBox(this);
    m_comboBox_type->setEnabled(false);
    m_comboBox_type->addItem("StandardOpaque");
    m_comboBox_type->addItem("AnisotropyOpaque");
    m_comboBox_type->addItem("IridescenceOpaque");
    m_comboBox_type->addItem("StandardTransparent");
    m_comboBox_type->addItem("AnisotropyTransparent");
    m_comboBox_type->addItem("IridescenceTransparent");
    m_layout_type->addWidget(m_label_type);
    m_layout_type->addWidget(m_comboBox_type);
    m_layout->addLayout(m_layout_type);

    // Base Color
    QHBoxLayout* hbox_baseColor = new QHBoxLayout();
    m_label_baseColor           = new QLabel("base_color:");
    m_label_baseColor->setFixedWidth(90);
    m_lineEdit_baseColor_r = new QLineEdit("255");
    m_lineEdit_baseColor_r->setValidator(new QIntValidator());
    m_lineEdit_baseColor_g = new QLineEdit("255");
    m_lineEdit_baseColor_g->setValidator(new QIntValidator());
    m_lineEdit_baseColor_b = new QLineEdit("255");
    m_lineEdit_baseColor_b->setValidator(new QIntValidator());
    hbox_baseColor->addWidget(m_label_baseColor);
    hbox_baseColor->addWidget(m_lineEdit_baseColor_r);
    hbox_baseColor->addWidget(m_lineEdit_baseColor_g);
    hbox_baseColor->addWidget(m_lineEdit_baseColor_b);
    m_layout->addLayout(hbox_baseColor);
    connect(m_lineEdit_baseColor_r, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_basecolor);
    connect(m_lineEdit_baseColor_g, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_basecolor);
    connect(m_lineEdit_baseColor_b, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_basecolor);

    // Alpha
    QHBoxLayout* hbox_alpha = new QHBoxLayout();
    m_label_alpha           = new QLabel("alpha:");
    m_label_alpha->setFixedWidth(90);
    m_lineEdit_alpha = new QLineEdit("0.0");
    m_lineEdit_alpha->setValidator(new QDoubleValidator());
    hbox_alpha->addWidget(m_label_alpha);
    hbox_alpha->addWidget(m_lineEdit_alpha);
    m_layout->addLayout(hbox_alpha);
    connect(m_lineEdit_alpha, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_alpha);

    // Roughness
    QHBoxLayout* hbox_roughness = new QHBoxLayout();
    m_label_roughness           = new QLabel("roughness:");
    m_label_roughness->setFixedWidth(90);
    m_lineEdit_roughness = new QLineEdit("0.0");
    m_lineEdit_roughness->setValidator(new QDoubleValidator());
    hbox_roughness->addWidget(m_label_roughness);
    hbox_roughness->addWidget(m_lineEdit_roughness);
    m_layout->addLayout(hbox_roughness);
    connect(m_lineEdit_roughness, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_roughness);

    // Metal
    QHBoxLayout* hbox_metal = new QHBoxLayout();
    m_label_metal           = new QLabel("metallic:");
    m_label_metal->setFixedWidth(90);
    m_lineEdit_metal = new QLineEdit("0.0");
    m_lineEdit_metal->setValidator(new QDoubleValidator());
    hbox_metal->addWidget(m_label_metal);
    hbox_metal->addWidget(m_lineEdit_metal);
    m_layout->addLayout(hbox_metal);
    connect(m_lineEdit_metal, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_metallic);

    // Emissive
    QHBoxLayout* hbox_emissive = new QHBoxLayout();
    m_label_emissive           = new QLabel("emission:");
    m_label_emissive->setFixedWidth(90);
    m_lineEdit_emissive_r = new QLineEdit("0");
    m_lineEdit_emissive_r->setValidator(new QIntValidator());
    m_lineEdit_emissive_g = new QLineEdit("0");
    m_lineEdit_emissive_g->setValidator(new QIntValidator());
    m_lineEdit_emissive_b = new QLineEdit("0");
    m_lineEdit_emissive_b->setValidator(new QIntValidator());
    hbox_emissive->addWidget(m_label_emissive);
    hbox_emissive->addWidget(m_lineEdit_emissive_r);
    hbox_emissive->addWidget(m_lineEdit_emissive_g);
    hbox_emissive->addWidget(m_lineEdit_emissive_b);
    m_layout->addLayout(hbox_emissive);
    connect(m_lineEdit_emissive_r, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_emissive);
    connect(m_lineEdit_emissive_g, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_emissive);
    connect(m_lineEdit_emissive_b, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_emissive);

    // Base Color Map
    QHBoxLayout* hbox_baseColorMap = new QHBoxLayout();
    m_label_basecolor_map          = new QLabel("baseColorMap:");
    m_label_basecolor_map->setFixedWidth(90);
    m_line_edit_basecolor_map = new QLineEdit();
    hbox_baseColorMap->addWidget(m_label_basecolor_map);
    hbox_baseColorMap->addWidget(m_line_edit_basecolor_map);
    m_layout->addLayout(hbox_baseColorMap);
    connect(m_line_edit_basecolor_map, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_basecolor_map);

    // Metal Rough Map
    QHBoxLayout* hbox_metalRoughMap = new QHBoxLayout();
    m_label_metal_rough_map         = new QLabel("metalRoughMap:");
    m_label_metal_rough_map->setFixedWidth(90);
    m_line_edit_metal_rough_map = new QLineEdit();
    hbox_metalRoughMap->addWidget(m_label_metal_rough_map);
    hbox_metalRoughMap->addWidget(m_line_edit_metal_rough_map);
    m_layout->addLayout(hbox_metalRoughMap);
    connect(m_line_edit_metal_rough_map, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_metal_rough_map);

    // Emissive Map
    QHBoxLayout* hbox_emissiveMap = new QHBoxLayout();
    m_label_emissive_map          = new QLabel("emissiveMap:");
    m_label_emissive_map->setFixedWidth(90);
    m_line_edit_emissive_map = new QLineEdit();
    hbox_emissiveMap->addWidget(m_label_emissive_map);
    hbox_emissiveMap->addWidget(m_line_edit_emissive_map);
    m_layout->addLayout(hbox_emissiveMap);
    connect(m_line_edit_emissive_map, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_emissivel_map);

    // Normal Map
    QHBoxLayout* hbox_normalMap = new QHBoxLayout();
    m_label_normal_map          = new QLabel("normalMap:");
    m_label_normal_map->setFixedWidth(90);
    m_line_edit_normal_map = new QLineEdit();
    hbox_normalMap->addWidget(m_label_normal_map);
    hbox_normalMap->addWidget(m_line_edit_normal_map);
    m_layout->addLayout(hbox_normalMap);
    connect(m_line_edit_normal_map, &QLineEdit::editingFinished, this, &MaterialWidget::_on_update_normal_map);

    m_layout->addStretch();
}

void MaterialWidget::set_material(uint32_t uuid)
{
    m_materialR = m_scene->find<acre::MaterialID>(uuid);
    if (!m_materialR || m_materialR->idx() == RESOURCE_ID_VALID) return;

    m_material = m_materialR->ptr<acre::MaterialID>();

    m_scene->unhighlight_material(m_materialR->id<acre::MaterialID>());
}

void MaterialWidget::update_properties()
{
    if (!m_material) return;
    _update_type();

    auto& model = std::get<acre::StandardModel>(m_material->model);

    // Common
    if (model.normal_idx.idx != -1)
        m_line_edit_normal_map->setText(QString::number(model.normal_idx.idx));
    else
        m_line_edit_normal_map->setText("-1");

    auto emission = model.emission;
    m_lineEdit_emissive_r->setText(QString::number(emission.x));
    m_lineEdit_emissive_g->setText(QString::number(emission.y));
    m_lineEdit_emissive_b->setText(QString::number(emission.z));
    if (model.emission_idx.idx != -1)
        m_line_edit_emissive_map->setText(QString::number(model.emission_idx.idx));
    else
        m_line_edit_emissive_map->setText("-1");

    m_lineEdit_alpha->setText(QString::number(m_material->alpha));

    acre::math::float3 base_color;
    uint32_t           base_color_idx;
    float              roughness;
    float              metallic;
    uint32_t           metalrough_idx;
    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
        {
            auto& model    = std::get<acre::StandardModel>(m_material->model);
            base_color     = model.base_color;
            base_color_idx = model.base_color_idx.idx;
            roughness      = model.roughness;
            metallic       = model.metallic;
            metalrough_idx = model.metalrough_idx.idx;
            break;
        }
    }

    m_lineEdit_baseColor_r->setText(QString::number(base_color.x));
    m_lineEdit_baseColor_g->setText(QString::number(base_color.y));
    m_lineEdit_baseColor_b->setText(QString::number(base_color.z));

    m_lineEdit_roughness->setText(QString::number(roughness));
    m_lineEdit_metal->setText(QString::number(metallic));

    if (base_color_idx != -1)
        m_line_edit_basecolor_map->setText(QString::number(base_color_idx));
    else
        m_line_edit_basecolor_map->setText("-1");

    if (metalrough_idx != -1)
        m_line_edit_metal_rough_map->setText(QString::number(metalrough_idx));
    else
        m_line_edit_metal_rough_map->setText("-1");
}

void MaterialWidget::_on_update_alpha()
{
    if (!m_material) return;
    auto value        = m_lineEdit_alpha->text();
    m_material->alpha = value.toFloat();

    _update_type();

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_emissive()
{
    if (!m_material) return;

    auto r = m_lineEdit_emissive_r->text();
    auto g = m_lineEdit_emissive_g->text();
    auto b = m_lineEdit_emissive_b->text();

    auto& model        = std::get<acre::StandardModel>(m_material->model);
    model.use_emission = true;
    model.emission.x   = r.toFloat();
    model.emission.y   = g.toFloat();
    model.emission.z   = b.toFloat();

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_normal_map()
{
    if (!m_material) return;
    auto  value = m_line_edit_normal_map->text();
    auto& model = std::get<acre::StandardModel>(m_material->model);
    // model.normal_map_scale    = 1.0f;
    model.normal_idx.idx = value.toFloat();

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_emissivel_map()
{
    if (!m_material) return;
    auto  value            = m_line_edit_emissive_map->text();
    auto& model            = std::get<acre::StandardModel>(m_material->model);
    model.emission_idx.idx = value.toFloat();

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_update_type()
{
    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
            if (m_material->use_alpha())
                m_comboBox_type->setCurrentIndex(3);
            else
                m_comboBox_type->setCurrentIndex(0);
            break;
    }
}

void MaterialWidget::_on_update_basecolor()
{
    if (!m_material) return;
    auto r = m_lineEdit_baseColor_r->text();
    auto g = m_lineEdit_baseColor_g->text();
    auto b = m_lineEdit_baseColor_b->text();

    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
        {
            auto& model        = std::get<acre::StandardModel>(m_material->model);
            model.base_color.x = r.toFloat();
            model.base_color.y = g.toFloat();
            model.base_color.z = b.toFloat();
            break;
        }
    }

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_roughness()
{
    if (!m_material) return;
    auto value = m_lineEdit_roughness->text();
    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
        {
            auto& model     = std::get<acre::StandardModel>(m_material->model);
            model.roughness = value.toFloat();
            break;
        }
    }

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_metallic()
{
    if (!m_material) return;
    auto value = m_lineEdit_metal->text();
    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
        {
            auto& model    = std::get<acre::StandardModel>(m_material->model);
            model.metallic = value.toFloat();
            break;
        }
    }

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_basecolor_map()
{
    if (!m_material) return;
    auto value = m_line_edit_basecolor_map->text();
    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
        {
            auto& model              = std::get<acre::StandardModel>(m_material->model);
            model.base_color_idx.idx = value.toFloat();
            break;
        }
    }

    m_scene->update(m_materialR);
    m_renderframe_func();
}

void MaterialWidget::_on_update_metal_rough_map()
{
    if (!m_material) return;
    auto value = m_line_edit_metal_rough_map->text();
    switch (m_material->type)
    {
        case acre::MaterialModel::mStandard:
        {
            auto& model              = std::get<acre::StandardModel>(m_material->model);
            model.metalrough_idx.idx = value.toFloat();
            break;
        }
    }

    m_scene->update(m_materialR);
    m_renderframe_func();
}
