#include <ui/sideBar/cameraWidget.h>
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
    auto eye = m_camera->getPosition();
    m_lineEdit_eye_x->setText(QString::number(eye.x));
    m_lineEdit_eye_y->setText(QString::number(eye.y));
    m_lineEdit_eye_z->setText(QString::number(eye.z));

    auto fov = m_camera->getFOV();
    m_lineEdit_fov->setText(QString::number(fov));

    auto near = m_camera->getNear();
    m_lineEdit_near->setText(QString::number(near));

    auto far = m_camera->getFar();
    m_lineEdit_far->setText(QString::number(far));
}

void CameraWidget::onUpdateEye()
{
    auto x = m_lineEdit_eye_x->text();
    auto y = m_lineEdit_eye_y->text();
    auto z = m_lineEdit_eye_z->text();

    m_camera->setPosition(rdr::math::float3(x.toFloat(), y.toFloat(), z.toFloat()));
    m_flushFrame();
}

void CameraWidget::onUpdateFov()
{
    auto value = m_lineEdit_fov->text();

    m_camera->setFOV(value.toFloat());
    m_flushFrame();
}

void CameraWidget::onUpdateNear()
{
    auto value = m_lineEdit_near->text();

    m_camera->setNear(value.toFloat());
    m_flushFrame();
}

void CameraWidget::onUpdateFar()
{
    auto value = m_lineEdit_far->text();

    m_camera->setFar(value.toFloat());
    m_flushFrame();
}