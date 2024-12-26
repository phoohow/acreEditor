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
    m_label_metal           = new QLabel("metal:");
    m_label_metal->setFixedWidth(90);
    m_lineEdit_metal = new QLineEdit("0.0");
    m_lineEdit_metal->setValidator(new QDoubleValidator());
    hbox_metal->addWidget(m_label_metal);
    hbox_metal->addWidget(m_lineEdit_metal);
    m_layout->addLayout(hbox_metal);
    connect(m_lineEdit_metal, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateMetallic);

    // Emissive
    QHBoxLayout* hbox_emissive = new QHBoxLayout();
    m_label_emissive           = new QLabel("emissive:");
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
    connect(m_lineEdit_emissive_r, &QLineEdit::editingFinished, this, &MaterialWidget::onUpdateEmissive);

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

void MaterialWidget::setMaterial(rdr::MaterialID id)
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

    auto basecolor = m_material->baseColor;
    m_lineEdit_baseColor_r->setText(QString::number(basecolor.x));
    m_lineEdit_baseColor_g->setText(QString::number(basecolor.y));
    m_lineEdit_baseColor_b->setText(QString::number(basecolor.z));

    m_lineEdit_alpha->setText(QString::number(basecolor.w));

    auto emissive = m_material->emissive;
    m_lineEdit_emissive_r->setText(QString::number(emissive.x));
    m_lineEdit_emissive_g->setText(QString::number(emissive.y));
    m_lineEdit_emissive_b->setText(QString::number(emissive.z));

    m_lineEdit_roughness->setText(QString::number(m_material->roughness));

    m_lineEdit_metal->setText(QString::number(m_material->metallic));

    if (m_material->baseColorIndex != -1)
        m_lineEdit_baseColorMap->setText(QString::number(m_material->baseColorIndex));
    else
        m_lineEdit_baseColorMap->setText("-1");

    if (m_material->normalIndex != -1)
        m_lineEdit_normalMap->setText(QString::number(m_material->normalIndex));
    else
        m_lineEdit_normalMap->setText("-1");

    if (m_material->emissiveIndex != -1)
        m_lineEdit_emissiveMap->setText(QString::number(m_material->emissiveIndex));
    else
        m_lineEdit_emissiveMap->setText("-1");

    if (m_material->metalRoughIndex != -1)
        m_lineEdit_metalRoughMap->setText(QString::number(m_material->metalRoughIndex));
    else
        m_lineEdit_metalRoughMap->setText("-1");
}

void MaterialWidget::updateType()
{
    switch (m_material->type)
    {
        case rdr::MaterialModel::Standard:
            if (m_material->isTransparent)
                m_comboBox_type->setCurrentIndex(3);
            else
                m_comboBox_type->setCurrentIndex(0);
            break;

        case rdr::MaterialModel::Anisotropy:
            if (m_material->isTransparent)
                m_comboBox_type->setCurrentIndex(4);
            else
                m_comboBox_type->setCurrentIndex(1);
            break;

        case rdr::MaterialModel::Iridescence:
            if (m_material->isTransparent)
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
    auto r = m_lineEdit_baseColor_r->text();
    auto g = m_lineEdit_baseColor_g->text();
    auto b = m_lineEdit_baseColor_b->text();

    m_material->baseColor.x = r.toFloat();
    m_material->baseColor.y = g.toFloat();
    m_material->baseColor.z = b.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateAlpha()
{
    auto value              = m_lineEdit_alpha->text();
    m_material->baseColor.w = value.toFloat();
    if (std::abs(m_material->baseColor.w) - 1 < 0)
        m_material->isTransparent = true;
    else
        m_material->isTransparent = false;
    updateType();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateRoughness()
{
    auto value            = m_lineEdit_roughness->text();
    m_material->roughness = value.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateMetallic()
{
    auto value           = m_lineEdit_metal->text();
    m_material->metallic = value.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateEmissive()
{
    auto r = m_lineEdit_emissive_r->text();
    auto g = m_lineEdit_emissive_g->text();
    auto b = m_lineEdit_emissive_b->text();

    m_material->emissive.x = r.toFloat();
    m_material->emissive.y = g.toFloat();
    m_material->emissive.z = b.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateBaseColorMap()
{
    auto value                 = m_lineEdit_baseColorMap->text();
    m_material->baseColorIndex = value.toFloat();

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
    m_material->emissiveIndex = value.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}

void MaterialWidget::onUpdateMetalRoughMap()
{
    auto value                  = m_lineEdit_metalRoughMap->text();
    m_material->metalRoughIndex = value.toFloat();

    m_scene->updateMaterial(m_materialID);
    m_flushFrame();
}
