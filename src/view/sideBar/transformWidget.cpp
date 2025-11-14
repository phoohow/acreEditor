#include <view/side/transformWidget.h>

#include <model/sceneMgr.h>

#include <QDoubleValidator>


TransformWidget::TransformWidget(SceneMgr* scene, QWidget* parent) :
    m_scene(scene),
    QWidget(parent)
{
    _init_ui();
}

TransformWidget::~TransformWidget()
{
    // Clean up if necessary
}

void TransformWidget::_init_ui()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);

    // Labels and line edits for translation with validators
    QLabel* label_translation = new QLabel("translation:");
    label_translation->setFixedWidth(90);
    m_lineEdit_translation_x                = new QLineEdit("0.000");
    m_lineEdit_translation_y                = new QLineEdit("0.000");
    m_lineEdit_translation_z                = new QLineEdit("0.000");
    QDoubleValidator* validator_translation = new QDoubleValidator(0.0, std::numeric_limits<double>::max(), 3, this);
    m_lineEdit_translation_x->setValidator(validator_translation);
    m_lineEdit_translation_y->setValidator(validator_translation);
    m_lineEdit_translation_z->setValidator(validator_translation);
    QHBoxLayout* hbox_translation = new QHBoxLayout();
    hbox_translation->addWidget(label_translation);
    hbox_translation->addWidget(m_lineEdit_translation_x);
    hbox_translation->addWidget(m_lineEdit_translation_y);
    hbox_translation->addWidget(m_lineEdit_translation_z);
    m_layout->addLayout(hbox_translation);
    connect(m_lineEdit_translation_x, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_translation);
    connect(m_lineEdit_translation_y, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_translation);
    connect(m_lineEdit_translation_z, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_translation);

    // Labels and line edits for rotation with validators
    QLabel* label_rotation = new QLabel("rotation:");
    label_rotation->setFixedWidth(90);
    m_lineEdit_rotation_x                = new QLineEdit("0.000");
    m_lineEdit_rotation_y                = new QLineEdit("0.000");
    m_lineEdit_rotation_z                = new QLineEdit("0.000");
    QDoubleValidator* validator_rotation = new QDoubleValidator(0.0, 360.0, 3, this);
    m_lineEdit_rotation_x->setValidator(validator_rotation);
    m_lineEdit_rotation_y->setValidator(validator_rotation);
    m_lineEdit_rotation_z->setValidator(validator_rotation);
    QHBoxLayout* hbox_rotation = new QHBoxLayout();
    hbox_rotation->addWidget(label_rotation);
    hbox_rotation->addWidget(m_lineEdit_rotation_x);
    hbox_rotation->addWidget(m_lineEdit_rotation_y);
    hbox_rotation->addWidget(m_lineEdit_rotation_z);
    m_layout->addLayout(hbox_rotation);
    connect(m_lineEdit_rotation_x, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_rotation);
    connect(m_lineEdit_rotation_y, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_rotation);
    connect(m_lineEdit_rotation_z, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_rotation);

    // Labels and line edits for scale with validators
    QLabel* label_scale = new QLabel("scale:");
    label_scale->setFixedWidth(90);
    m_lineEdit_scale_x                = new QLineEdit("1.000");
    m_lineEdit_scale_y                = new QLineEdit("1.000");
    m_lineEdit_scale_z                = new QLineEdit("1.000");
    QDoubleValidator* validator_scale = new QDoubleValidator(0.0, std::numeric_limits<double>::max(), 3, this);
    m_lineEdit_scale_x->setValidator(validator_scale);
    m_lineEdit_scale_y->setValidator(validator_scale);
    m_lineEdit_scale_z->setValidator(validator_scale);
    QHBoxLayout* hbox_scale = new QHBoxLayout();
    hbox_scale->addWidget(label_scale);
    hbox_scale->addWidget(m_lineEdit_scale_x);
    hbox_scale->addWidget(m_lineEdit_scale_y);
    hbox_scale->addWidget(m_lineEdit_scale_z);
    m_layout->addLayout(hbox_scale);
    connect(m_lineEdit_scale_x, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_scale);
    connect(m_lineEdit_scale_y, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_scale);
    connect(m_lineEdit_scale_z, &QLineEdit::editingFinished, this, &TransformWidget::_on_update_scale);

    // Adjust the layout to fit the widget
    m_layout->addStretch();
}

void TransformWidget::set_transform(uint32_t uuid)
{
    m_transformR = m_scene->find<acre::TransformID>(uuid);
    m_transform  = m_transformR->ptr<acre::TransformID>();
}

void TransformWidget::update_properties()
{
    // Example: Assuming there is a 'Transform' object that holds the properties
    // Transform* transform = ...; // Get the transform object from somewhere

    // Update the line edits with the current transform properties
    // For example:
    // m_lineEdit_translation_x->setText(QString::number(transform->translation.x));
    // m_lineEdit_translation_y->setText(QString::number(transform->translation.y));
    // m_lineEdit_translation_z->setText(QString::number(transform->translation.z));
    // Repeat for rotation and scale

    // Since we don't have the actual 'Transform' object and its properties, the above is just a placeholder
    // You would replace this with the actual code to retrieve and set the values from your transform object
}


void TransformWidget::_on_update_translation()
{
    // Example: Assuming there is a 'Transform' object that holds the properties
    // Transform* transform = ...; // Get the transform object from somewhere

    // Update the transform properties with the values from the line edits
    // For example:
    // transform->translation.x = m_lineEdit_translation_x->text().toFloat();
    // transform->translation.y = m_lineEdit_translation_y->text().toFloat();
    // transform->translation.z = m_lineEdit_translation_z->text().toFloat();

    // Since we don't have the actual 'Transform' object and its properties, the above is just a placeholder
    // You would replace this with the actual code to update the transform properties
}

void TransformWidget::_on_update_rotation()
{
    // Example: Assuming there is a 'Transform' object that holds the properties
    // Transform* transform = ...; // Get the transform object from somewhere

    // Update the transform properties with the values from the line edits
    // For example:
    // transform->rotation.x = m_lineEdit_rotation_x->text().toFloat();
    // transform->rotation.y = m_lineEdit_rotation_y->text().toFloat();
    // transform->rotation.z = m_lineEdit_rotation_z->text().toFloat();

    // Since we don't have the actual 'Transform' object and its properties, the above is just a placeholder
    // You would replace this with the actual code to update the transform properties
}

void TransformWidget::_on_update_scale()
{
    // Example: Assuming there is a 'Transform' object that holds the properties
    // Transform* transform = ...; // Get the transform object from somewhere

    // Update the transform properties with the values from the line edits
    // For example:
    // transform->scale.x = m_lineEdit_scale_x->text().toFloat();
    // transform->scale.y = m_lineEdit_scale_y->text().toFloat();
    // transform->scale.z = m_lineEdit_scale_z->text().toFloat();

    // Since we don't have the actual 'Transform' object and its properties, the above is just a placeholder
    // You would replace this with the actual code to update the transform properties
}
