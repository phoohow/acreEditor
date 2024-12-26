#include <ui/sideBar/materialWidget.h>
#include <scene/gltfScene.h>

#include <QDoubleValidator>
#include <QIntValidator>

MaterialWidget::MaterialWidget(BaseScene* scene, QWidget* parent) :
    m_scene(scene),
    QWidget(parent)
{
    initUI();
}

MaterialWidget::~MaterialWidget()
{
    // Clean up if necessary
}

void MaterialWidget::initUI()
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
    m_label_baseColor           = new QLabel("baseColor:");
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
    connect(m_lineEdit_baseColor_r, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateBaseColor);
    connect(m_lineEdit_baseColor_g, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateBaseColor);
    connect(m_lineEdit_baseColor_b, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateBaseColor);

    // Alpha
    QHBoxLayout* hbox_alpha = new QHBoxLayout();
    m_label_alpha           = new QLabel("alpha:");
    m_label_alpha->setFixedWidth(90);
    m_lineEdit_alpha = new QLineEdit("0.0");
    m_lineEdit_alpha->setValidator(new QDoubleValidator());
    hbox_alpha->addWidget(m_label_alpha);
    hbox_alpha->addWidget(m_lineEdit_alpha);
    m_layout->addLayout(hbox_alpha);
    connect(m_lineEdit_alpha, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateAlpha);

    // Roughness
    QHBoxLayout* hbox_roughness = new QHBoxLayout();
    m_label_roughness           = new QLabel("roughness:");
    m_label_roughness->setFixedWidth(90);
    m_lineEdit_roughness = new QLineEdit("0.0");
    m_lineEdit_roughness->setValidator(new QDoubleValidator());
    hbox_roughness->addWidget(m_label_roughness);
    hbox_roughness->addWidget(m_lineEdit_roughness);
    m_layout->addLayout(hbox_roughness);
    connect(m_lineEdit_roughness, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateRoughness);

    // Metal
    QHBoxLayout* hbox_metal = new QHBoxLayout();
    m_label_metal           = new QLabel("metallic:");
    m_label_metal->setFixedWidth(90);
    m_lineEdit_metal = new QLineEdit("0.0");
    m_lineEdit_metal->setValidator(new QDoubleValidator());
    hbox_metal->addWidget(m_label_metal);
    hbox_metal->addWidget(m_lineEdit_metal);
    m_layout->addLayout(hbox_metal);
    connect(m_lineEdit_metal, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateMetallic);

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
    connect(m_lineEdit_emissive_r, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateEmissive);
    connect(m_lineEdit_emissive_g, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateEmissive);
    connect(m_lineEdit_emissive_b, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateEmissive);

    // Base Color Map
    QHBoxLayout* hbox_baseColorMap = new QHBoxLayout();
    m_label_baseColorMap           = new QLabel("baseColorMap:");
    m_label_baseColorMap->setFixedWidth(90);
    m_lineEdit_baseColorMap = new QLineEdit();
    hbox_baseColorMap->addWidget(m_label_baseColorMap);
    hbox_baseColorMap->addWidget(m_lineEdit_baseColorMap);
    m_layout->addLayout(hbox_baseColorMap);
    connect(m_lineEdit_baseColorMap, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateBaseColorMap);

    // Metal Rough Map
    QHBoxLayout* hbox_metalRoughMap = new QHBoxLayout();
    m_label_metalRoughMap           = new QLabel("metalRoughMap:");
    m_label_metalRoughMap->setFixedWidth(90);
    m_lineEdit_metalRoughMap = new QLineEdit();
    hbox_metalRoughMap->addWidget(m_label_metalRoughMap);
    hbox_metalRoughMap->addWidget(m_lineEdit_metalRoughMap);
    m_layout->addLayout(hbox_metalRoughMap);
    connect(m_lineEdit_metalRoughMap, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateMetalRoughMap);

    // Emissive Map
    QHBoxLayout* hbox_emissiveMap = new QHBoxLayout();
    m_label_emissiveMap           = new QLabel("emissiveMap:");
    m_label_emissiveMap->setFixedWidth(90);
    m_lineEdit_emissiveMap = new QLineEdit();
    hbox_emissiveMap->addWidget(m_label_emissiveMap);
    hbox_emissiveMap->addWidget(m_lineEdit_emissiveMap);
    m_layout->addLayout(hbox_emissiveMap);
    connect(m_lineEdit_emissiveMap, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateEmissiveMap);

    // Normal Map
    QHBoxLayout* hbox_normalMap = new QHBoxLayout();
    m_label_normalMap           = new QLabel("normalMap:");
    m_label_normalMap->setFixedWidth(90);
    m_lineEdit_normalMap = new QLineEdit();
    hbox_normalMap->addWidget(m_label_normalMap);
    hbox_normalMap->addWidget(m_lineEdit_normalMap);
    m_layout->addLayout(hbox_normalMap);
    connect(m_lineEdit_normalMap, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateNormalMap);

    m_layout->addStretch();
}

void MaterialWidget::setMaterial(acre::MaterialID id)
{
    if (m_materialID != -1)
        m_scene->unhighlightMaterial(m_materialID);

    m_materialID = id;
    m_material   = m_scene->getMaterial(id);
}

void MaterialWidget::updateProperties()
{
    if (!m_material) return;
    updateType();

    // Common
    if (m_material->normalIndex != -1)
        m_lineEdit_normalMap->setText(QString::number(m_material->normalIndex));
    else
        m_lineEdit_normalMap->setText("-1");

    auto emission = m_material->emission;
    m_lineEdit_emissive_r->setText(QString::number(emission.x));
    m_lineEdit_emissive_g->setText(QString::number(emission.y));
    m_lineEdit_emissive_b->setText(QString::number(emission.z));
    if (m_material->emissionIndex != -1)
        m_lineEdit_emissiveMap->setText(QString::number(m_material->emissionIndex));
    else
        m_lineEdit_emissiveMap->setText("-1");

    m_lineEdit_alpha->setText(QString::number(m_material->alpha));

    acre::math::float3 baseColor;
    uint32_t           baseColorIndex;
    float              roughness;
    float              metallic;
    uint32_t           metalRoughIndex;
    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
        {
            auto& surface   = std::get<acre::SurfaceStandard>(m_material->surface);
            baseColor       = surface.baseColor;
            baseColorIndex  = surface.baseColorIndex;
            roughness       = surface.roughness;
            metallic        = surface.metallic;
            metalRoughIndex = surface.metalRoughIndex;
            break;
        }
        case acre::MaterialSurfaceModel::Anisotropy:
        {
            auto& surface   = std::get<acre::SurfaceAnisotropy>(m_material->surface);
            baseColor       = surface.baseColor;
            baseColorIndex  = surface.baseColorIndex;
            roughness       = surface.roughness;
            metallic        = surface.metallic;
            metalRoughIndex = surface.metalRoughIndex;
            // TODO: anisotropy
            break;
        }
        case acre::MaterialSurfaceModel::Iridescence:
        {
            auto& surface   = std::get<acre::SurfaceIrridescence>(m_material->surface);
            baseColor       = surface.baseColor;
            baseColorIndex  = surface.baseColorIndex;
            roughness       = surface.roughness;
            metallic        = surface.metallic;
            metalRoughIndex = surface.metalRoughIndex;
            // TODO: iridescence
            break;
        }
    }

    m_lineEdit_baseColor_r->setText(QString::number(baseColor.x));
    m_lineEdit_baseColor_g->setText(QString::number(baseColor.y));
    m_lineEdit_baseColor_b->setText(QString::number(baseColor.z));

    m_lineEdit_roughness->setText(QString::number(roughness));
    m_lineEdit_metal->setText(QString::number(metallic));

    if (baseColorIndex != -1)
        m_lineEdit_baseColorMap->setText(QString::number(baseColorIndex));
    else
        m_lineEdit_baseColorMap->setText("-1");

    if (metalRoughIndex != -1)
        m_lineEdit_metalRoughMap->setText(QString::number(metalRoughIndex));
    else
        m_lineEdit_metalRoughMap->setText("-1");
}

void MaterialWidget::onUpdateAlpha()
{
    auto value        = m_lineEdit_alpha->text();
    m_material->alpha = value.toFloat();

    updateType();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateEmissive()
{
    auto r = m_lineEdit_emissive_r->text();
    auto g = m_lineEdit_emissive_g->text();
    auto b = m_lineEdit_emissive_b->text();

    m_material->emission.x = r.toFloat();
    m_material->emission.y = g.toFloat();
    m_material->emission.z = b.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateNormalMap()
{
    auto value              = m_lineEdit_normalMap->text();
    m_material->normalIndex = value.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateEmissiveMap()
{
    auto value                = m_lineEdit_emissiveMap->text();
    m_material->emissionIndex = value.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::updateType()
{
    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
            if (m_material->useAlpha())
                m_comboBox_type->setCurrentIndex(3);
            else
                m_comboBox_type->setCurrentIndex(0);
            break;

        case acre::MaterialSurfaceModel::Anisotropy:
            if (m_material->useAlpha())
                m_comboBox_type->setCurrentIndex(4);
            else
                m_comboBox_type->setCurrentIndex(1);
            break;

        case acre::MaterialSurfaceModel::Iridescence:
            if (m_material->useAlpha())
                m_comboBox_type->setCurrentIndex(5);
            else
                m_comboBox_type->setCurrentIndex(2);
            break;
        default:
            break;
    }
}

void MaterialWidget::onUpdateBaseColor()
{
    if (!m_material) return;
    auto r = m_lineEdit_baseColor_r->text();
    auto g = m_lineEdit_baseColor_g->text();
    auto b = m_lineEdit_baseColor_b->text();

    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
        {
            auto& surface       = std::get<acre::SurfaceStandard>(m_material->surface);
            surface.baseColor.x = r.toFloat();
            surface.baseColor.y = g.toFloat();
            surface.baseColor.z = b.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Anisotropy:
        {
            auto& surface       = std::get<acre::SurfaceAnisotropy>(m_material->surface);
            surface.baseColor.x = r.toFloat();
            surface.baseColor.y = g.toFloat();
            surface.baseColor.z = b.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Iridescence:
        {
            auto& surface       = std::get<acre::SurfaceIrridescence>(m_material->surface);
            surface.baseColor.x = r.toFloat();
            surface.baseColor.y = g.toFloat();
            surface.baseColor.z = b.toFloat();
            break;
        }
    }

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateRoughness()
{
    auto value = m_lineEdit_roughness->text();
    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
        {
            auto& surface     = std::get<acre::SurfaceStandard>(m_material->surface);
            surface.roughness = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Anisotropy:
        {
            auto& surface     = std::get<acre::SurfaceAnisotropy>(m_material->surface);
            surface.roughness = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Iridescence:
        {
            auto& surface     = std::get<acre::SurfaceIrridescence>(m_material->surface);
            surface.roughness = value.toFloat();
            break;
        }
    }

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateMetallic()
{
    auto value = m_lineEdit_metal->text();
    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
        {
            auto& surface    = std::get<acre::SurfaceStandard>(m_material->surface);
            surface.metallic = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Anisotropy:
        {
            auto& surface    = std::get<acre::SurfaceAnisotropy>(m_material->surface);
            surface.metallic = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Iridescence:
        {
            auto& surface    = std::get<acre::SurfaceIrridescence>(m_material->surface);
            surface.metallic = value.toFloat();
            break;
        }
    }

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateBaseColorMap()
{
    auto value = m_lineEdit_baseColorMap->text();
    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
        {
            auto& surface          = std::get<acre::SurfaceStandard>(m_material->surface);
            surface.baseColorIndex = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Anisotropy:
        {
            auto& surface          = std::get<acre::SurfaceAnisotropy>(m_material->surface);
            surface.baseColorIndex = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Iridescence:
        {
            auto& surface          = std::get<acre::SurfaceIrridescence>(m_material->surface);
            surface.baseColorIndex = value.toFloat();
            break;
        }
    }

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateMetalRoughMap()
{
    auto value = m_lineEdit_metalRoughMap->text();
    switch (m_material->surfaceModel)
    {
        case acre::MaterialSurfaceModel::Standard:
        {
            auto& surface           = std::get<acre::SurfaceStandard>(m_material->surface);
            surface.metalRoughIndex = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Anisotropy:
        {
            auto& surface           = std::get<acre::SurfaceAnisotropy>(m_material->surface);
            surface.metalRoughIndex = value.toFloat();
            break;
        }

        case acre::MaterialSurfaceModel::Iridescence:
        {
            auto& surface           = std::get<acre::SurfaceIrridescence>(m_material->surface);
            surface.metalRoughIndex = value.toFloat();
            break;
        }
    }

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}
