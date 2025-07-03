#include <view/side/cameraWidget.h>
#include <QDoubleValidator>

CameraWidget::CameraWidget(QWidget* parent) :
    QWidget(parent)
{
    initUI();
}

CameraWidget::~CameraWidget()
{
    // Clean up if necessary
}

void CameraWidget::initUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);

    QHBoxLayout* hbox_eye = new QHBoxLayout();
    m_label_eye           = new QLabel("eye:");
    m_label_eye->setFixedWidth(90);
    m_lineEdit_eye_x = new QLineEdit("0.0");
    m_lineEdit_eye_x->setValidator(new QDoubleValidator());
    m_lineEdit_eye_y = new QLineEdit("0.0");
    m_lineEdit_eye_y->setValidator(new QDoubleValidator());
    m_lineEdit_eye_z = new QLineEdit("0.0");
    m_lineEdit_eye_z->setValidator(new QDoubleValidator());
    hbox_eye->addWidget(m_label_eye);
    hbox_eye->addWidget(m_lineEdit_eye_x);
    hbox_eye->addWidget(m_lineEdit_eye_y);
    hbox_eye->addWidget(m_lineEdit_eye_z);
    hbox_eye->setSpacing(5);
    m_layout->addLayout(hbox_eye);
    connect(m_lineEdit_eye_x, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateEye);
    connect(m_lineEdit_eye_y, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateEye);
    connect(m_lineEdit_eye_z, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateEye);

    QHBoxLayout* hbox_target = new QHBoxLayout();
    m_label_target           = new QLabel("target:");
    m_label_target->setFixedWidth(90);
    m_lineEdit_target_x = new QLineEdit("0.0");
    m_lineEdit_target_x->setValidator(new QDoubleValidator());
    m_lineEdit_target_y = new QLineEdit("0.0");
    m_lineEdit_target_y->setValidator(new QDoubleValidator());
    m_lineEdit_target_z = new QLineEdit("0.0");
    m_lineEdit_target_z->setValidator(new QDoubleValidator());
    hbox_target->addWidget(m_label_target);
    hbox_target->addWidget(m_lineEdit_target_x);
    hbox_target->addWidget(m_lineEdit_target_y);
    hbox_target->addWidget(m_lineEdit_target_z);
    hbox_target->setSpacing(5);
    m_layout->addLayout(hbox_target);
    connect(m_lineEdit_target_x, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateTarget);
    connect(m_lineEdit_target_y, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateTarget);
    connect(m_lineEdit_target_z, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateTarget);

    QHBoxLayout* hbox_fov = new QHBoxLayout();
    m_label_fov           = new QLabel("fov:");
    m_label_fov->setFixedWidth(90);
    m_lineEdit_fov = new QLineEdit("0.0");
    m_lineEdit_fov->setValidator(new QDoubleValidator());
    hbox_fov->addWidget(m_label_fov);
    hbox_fov->addWidget(m_lineEdit_fov);
    hbox_fov->setSpacing(5);
    m_layout->addLayout(hbox_fov);
    connect(m_lineEdit_fov, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateFov);

    QHBoxLayout* hbox_near = new QHBoxLayout();
    m_label_near           = new QLabel("near:");
    m_label_near->setFixedWidth(90);
    m_lineEdit_near = new QLineEdit("0.0");
    m_lineEdit_near->setValidator(new QDoubleValidator());
    hbox_near->addWidget(m_label_near);
    hbox_near->addWidget(m_lineEdit_near);
    hbox_near->setSpacing(5);
    m_layout->addLayout(hbox_near);
    connect(m_lineEdit_near, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateNear);

    QHBoxLayout* hbox_far = new QHBoxLayout();
    m_label_far           = new QLabel("far:");
    m_label_far->setFixedWidth(90);
    m_lineEdit_far = new QLineEdit("0.0");
    m_lineEdit_far->setValidator(new QDoubleValidator());
    hbox_far->addWidget(m_label_far);
    hbox_far->addWidget(m_lineEdit_far);
    hbox_far->setSpacing(5);
    m_layout->addLayout(hbox_far);
    connect(m_lineEdit_far, &QLineEdit::editingFinished, this, &CameraWidget::onUpdateFar);

    // Adjust the layout to fit the widget
    m_layout->addStretch();
}

void CameraWidget::updateProperties()
{
    const auto& eye = m_camera->position;
    m_lineEdit_eye_x->setText(QString::number(eye.x));
    m_lineEdit_eye_y->setText(QString::number(eye.y));
    m_lineEdit_eye_z->setText(QString::number(eye.z));

    const auto& target = m_camera->target;
    m_lineEdit_target_x->setText(QString::number(target.x));
    m_lineEdit_target_y->setText(QString::number(target.y));
    m_lineEdit_target_z->setText(QString::number(target.z));

    if (m_camera->type == acre::Camera::ProjectType::tPerspective)
    {
        const auto& projection = std::get<acre::Camera::Perspective>(m_camera->projection);
        m_lineEdit_fov->setText(QString::number(projection.fov));
        m_lineEdit_near->setText(QString::number(projection.nearPlane));
        m_lineEdit_far->setText(QString::number(projection.farPlane));
    }
}

void CameraWidget::onUpdateEye()
{
    auto x = m_lineEdit_eye_x->text();
    auto y = m_lineEdit_eye_y->text();
    auto z = m_lineEdit_eye_z->text();

    m_camera->position = acre::math::float3(x.toFloat(), y.toFloat(), z.toFloat());
    m_renderFrameFunc();
}

void CameraWidget::onUpdateTarget()
{
    auto x = m_lineEdit_target_x->text();
    auto y = m_lineEdit_target_y->text();
    auto z = m_lineEdit_target_z->text();

    m_camera->target = acre::math::float3(x.toFloat(), y.toFloat(), z.toFloat());
    m_renderFrameFunc();
}

void CameraWidget::onUpdateFov()
{
    auto value = m_lineEdit_fov->text();

    if (m_camera->type == acre::Camera::ProjectType::tPerspective)
    {
        auto& projection = std::get<acre::Camera::Perspective>(m_camera->projection);
        projection.fov   = value.toFloat();
    }

    m_renderFrameFunc();
}

void CameraWidget::onUpdateNear()
{
    auto value = m_lineEdit_near->text();

    if (m_camera->type == acre::Camera::ProjectType::tPerspective)
    {
        auto& projection     = std::get<acre::Camera::Perspective>(m_camera->projection);
        projection.nearPlane = value.toFloat();
    }

    m_renderFrameFunc();
}

void CameraWidget::onUpdateFar()
{
    auto value = m_lineEdit_far->text();

    if (m_camera->type == acre::Camera::ProjectType::tPerspective)
    {
        auto& projection    = std::get<acre::Camera::Perspective>(m_camera->projection);
        projection.farPlane = value.toFloat();
    }

    m_renderFrameFunc();
}