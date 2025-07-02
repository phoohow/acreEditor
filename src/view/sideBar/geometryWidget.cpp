#include <view/side/geometryWidget.h>

#include <model/sceneMgr.h>

#include <QDoubleValidator>

GeometryWidget::GeometryWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene),
    QWidget(parent)
{
    initUI();
}

GeometryWidget::~GeometryWidget()
{
    // Clean up if necessary
}

void GeometryWidget::initUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);

    // Checkboxes for attributes
    m_checkbox_index = new QCheckBox("index");
    m_checkbox_index->setDisabled(true);
    // m_checkbox_index->setStyleSheet("QCheckBox::indicator:checked { background-color: lightGreen; border: 1px solid lightGray; }");
    m_checkbox_position = new QCheckBox("position");
    m_checkbox_position->setDisabled(true);
    m_checkbox_uv = new QCheckBox("uv");
    m_checkbox_uv->setDisabled(true);
    m_checkbox_normal = new QCheckBox("normal");
    m_checkbox_normal->setDisabled(true);
    m_checkbox_tangent = new QCheckBox("tangent");
    m_checkbox_tangent->setDisabled(true);
    m_checkbox_color = new QCheckBox("color");
    m_checkbox_color->setDisabled(true);
    m_layout->addWidget(m_checkbox_index);
    m_layout->addWidget(m_checkbox_position);
    m_layout->addWidget(m_checkbox_uv);
    m_layout->addWidget(m_checkbox_normal);
    m_layout->addWidget(m_checkbox_tangent);
    m_layout->addWidget(m_checkbox_color);

    // Labels and line edits for indexCount and vertexCount
    QLabel* label_indexCount = new QLabel("indexCount:");
    label_indexCount->setFixedWidth(90);
    m_lineEdit_indexCount = new QLineEdit();
    m_lineEdit_indexCount->setReadOnly(true);
    QHBoxLayout* hbox_indexCount = new QHBoxLayout();
    hbox_indexCount->addWidget(label_indexCount);
    hbox_indexCount->addWidget(m_lineEdit_indexCount);
    m_layout->addLayout(hbox_indexCount);

    QLabel* label_vertexCount = new QLabel("vertexCount:");
    label_vertexCount->setFixedWidth(90);
    m_lineEdit_vertexCount = new QLineEdit();
    m_lineEdit_vertexCount->setReadOnly(true);
    QHBoxLayout* hbox_vertexCount = new QHBoxLayout();
    hbox_vertexCount->addWidget(label_vertexCount);
    hbox_vertexCount->addWidget(m_lineEdit_vertexCount);
    m_layout->addLayout(hbox_vertexCount);

    // Adjust the layout to fit the widget
    m_layout->addStretch();
}

void GeometryWidget::setGeometry(acre::GeometryID id)
{
    if (m_geometryID != -1)
        m_scene->unhighlightGeometry(m_geometryID);

    m_geometryID = id;
    m_geometry   = m_scene->getGeometry(id);
}

void GeometryWidget::updateProperties()
{
    if (!m_geometry) return;
    m_checkbox_index->setChecked(m_geometry->index != RESOURCE_ID_VALID);
    m_checkbox_position->setChecked(m_geometry->position != RESOURCE_ID_VALID);
    m_checkbox_uv->setChecked(m_geometry->uv != RESOURCE_ID_VALID);
    m_checkbox_normal->setChecked(m_geometry->normal != RESOURCE_ID_VALID);
    m_checkbox_tangent->setChecked(m_geometry->tangent != RESOURCE_ID_VALID);
    m_checkbox_color->setChecked(m_geometry->color != RESOURCE_ID_VALID);

    auto indexCount = m_scene->getVIndexBuffer(m_geometry->index)->count;
    m_lineEdit_indexCount->setText(QString::number(indexCount));

    auto vertexCount = m_scene->getVPositionBuffer(m_geometry->position)->count;
    m_lineEdit_vertexCount->setText(QString::number(vertexCount));
}