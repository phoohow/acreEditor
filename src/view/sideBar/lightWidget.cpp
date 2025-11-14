#include <view/side/lightWidget.h>

#include <model/sceneMgr.h>

#include <QDoubleValidator>
#include <QIntValidator>

LightWidget::LightWidget(SceneMgr* scene, QWidget* parent) :
    QWidget(parent),
    m_scene(scene),
    m_type(LightType::Sun)
{
    _init_ui();
}

void LightWidget::_on_update_type(int index)
{
    LightWidget::LightType newType = static_cast<LightWidget::LightType>(index);
    if (m_type != newType)
    {
        m_type = newType;
        update_properties();
    }
}

void LightWidget::set_light(uint32_t uuid)
{
    m_lightR = m_scene->find<acre::LightID>(uuid);
    m_light  = m_lightR->ptr<acre::LightID>();
}

void LightWidget::enable_sun()
{
    m_useSun = true;
}

void LightWidget::disable_sun()
{
    m_useSun = false;
}

void LightWidget::enable_hdr()
{
    m_useHDR = true;
}

void LightWidget::disable_hdr()
{
    m_useHDR = false;
}

void LightWidget::_init_ui()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(10);

    m_layout_type = new QHBoxLayout();
    m_label_type  = new QLabel("type:");
    m_label_type->setFixedWidth(90);
    m_comboBox_type = new QComboBox(this);
    m_comboBox_type->addItem("Sun");
    m_comboBox_type->addItem("HDR");
    m_comboBox_type->addItem("Point");
    m_layout_type->addWidget(m_label_type);
    m_layout_type->addWidget(m_comboBox_type);
    m_layout->addLayout(m_layout_type);
    connect(m_comboBox_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LightWidget::_on_update_type);

    m_layout_intensity = new QHBoxLayout();
    m_label_intensity  = new QLabel("factor:");
    m_label_intensity->setFixedWidth(90);
    m_lineEdit_intensity = new QLineEdit("1.0");
    m_lineEdit_intensity->setValidator(new QDoubleValidator());
    m_layout_intensity->addWidget(m_label_intensity);
    m_layout_intensity->addWidget(m_lineEdit_intensity);
    m_layout->addLayout(m_layout_intensity);
    connect(m_lineEdit_intensity, &QLineEdit::editingFinished, this, &LightWidget::_on_update_intensity);

    m_layout_color = new QHBoxLayout();
    m_label_color  = new QLabel("color:");
    m_label_color->setFixedWidth(90);
    m_lineEdit_color_r = new QLineEdit("255");
    m_lineEdit_color_r->setValidator(new QDoubleValidator());
    m_lineEdit_color_g = new QLineEdit("255");
    m_lineEdit_color_g->setValidator(new QDoubleValidator());
    m_lineEdit_color_b = new QLineEdit("255");
    m_lineEdit_color_b->setValidator(new QDoubleValidator());
    m_layout_color->addWidget(m_label_color);
    m_layout_color->addWidget(m_lineEdit_color_r);
    m_layout_color->addWidget(m_lineEdit_color_g);
    m_layout_color->addWidget(m_lineEdit_color_b);
    m_layout->addLayout(m_layout_color);
    connect(m_lineEdit_color_r, &QLineEdit::editingFinished, this, &LightWidget::_on_update_color);
    connect(m_lineEdit_color_g, &QLineEdit::editingFinished, this, &LightWidget::_on_update_color);
    connect(m_lineEdit_color_b, &QLineEdit::editingFinished, this, &LightWidget::_on_update_color);

    _init_direction_ui();

    _init_image_ui();

    _init_point_ui();

    _hide_hdr_ui();

    _hide_point_ui();

    // Adjust the layout to fit the widget
    m_layout->addStretch();
}

void LightWidget::_init_direction_ui()
{
    m_layout_direction = new QHBoxLayout();
    m_label_direction  = new QLabel("direction:");
    m_label_direction->setFixedWidth(90);
    m_lineEdit_direction_x = new QLineEdit("0.0");
    m_lineEdit_direction_x->setValidator(new QDoubleValidator());
    m_lineEdit_direction_y = new QLineEdit("1.0");
    m_lineEdit_direction_y->setValidator(new QDoubleValidator());
    m_lineEdit_direction_z = new QLineEdit("0.0");
    m_lineEdit_direction_z->setValidator(new QDoubleValidator());
    m_layout_direction->addWidget(m_label_direction);
    m_layout_direction->addWidget(m_lineEdit_direction_x);
    m_layout_direction->addWidget(m_lineEdit_direction_y);
    m_layout_direction->addWidget(m_lineEdit_direction_z);
    m_layout->addLayout(m_layout_direction);

    connect(m_lineEdit_direction_x, &QLineEdit::editingFinished, this, &LightWidget::_on_update_direction);
    connect(m_lineEdit_direction_y, &QLineEdit::editingFinished, this, &LightWidget::_on_update_direction);
    connect(m_lineEdit_direction_z, &QLineEdit::editingFinished, this, &LightWidget::_on_update_direction);
}

void LightWidget::_init_image_ui()
{
    m_layout_texture = new QHBoxLayout();
    m_label_texture  = new QLabel("texture:");
    m_label_texture->setFixedWidth(90);
    m_lineEdit_texture = new QLineEdit("0");
    m_lineEdit_texture->setValidator(new QIntValidator());
    m_layout_texture->addWidget(m_label_texture);
    m_layout_texture->addWidget(m_lineEdit_texture);
    m_layout->addLayout(m_layout_texture);

    connect(m_lineEdit_texture, &QLineEdit::editingFinished, this, &LightWidget::_on_update_texture);
}

void LightWidget::_init_point_ui()
{
    m_layout_position = new QHBoxLayout();
    m_label_position  = new QLabel("position:");
    m_label_position->setFixedWidth(90);
    m_lineEdit_position_x = new QLineEdit("0.0");
    m_lineEdit_position_x->setValidator(new QDoubleValidator());
    m_lineEdit_position_y = new QLineEdit("0.0");
    m_lineEdit_position_y->setValidator(new QDoubleValidator());
    m_lineEdit_position_z = new QLineEdit("0.0");
    m_lineEdit_position_z->setValidator(new QDoubleValidator());
    m_layout_position->addWidget(m_label_position);
    m_layout_position->addWidget(m_lineEdit_position_x);
    m_layout_position->addWidget(m_lineEdit_position_y);
    m_layout_position->addWidget(m_lineEdit_position_z);
    m_layout->addLayout(m_layout_position);

    connect(m_lineEdit_position_x, &QLineEdit::editingFinished, this, &LightWidget::_on_update_position);
    connect(m_lineEdit_position_y, &QLineEdit::editingFinished, this, &LightWidget::_on_update_position);
    connect(m_lineEdit_position_z, &QLineEdit::editingFinished, this, &LightWidget::_on_update_position);
}

void LightWidget::update_properties()
{
    if (!m_light) return;
    switch (m_light->type)
    {
        case acre::LightType::Point:
            m_type = LightType::Point;
            m_comboBox_type->setCurrentIndex(Point);
            _show_point_ui();
            _hide_direction_ui();
            _hide_hdr_ui();
            break;
        default:
            break;
    }
}

void LightWidget::update_sun_properties()
{
    m_type = LightType::Sun;
    m_comboBox_type->setCurrentIndex(Sun);

    _show_sun_ui();
    _hide_hdr_ui();
    _hide_point_ui();
}

void LightWidget::update_hdr_properties()
{
    m_type = LightType::HDR;
    m_comboBox_type->setCurrentIndex(HDR);

    _show_hdr_ui();
    _hide_direction_ui();
    _hide_point_ui();
}

void LightWidget::_show_sun_ui()
{
    m_label_direction->show();
    m_lineEdit_direction_x->show();
    m_lineEdit_direction_y->show();
    m_lineEdit_direction_z->show();

    auto light = m_scene->get_sun_light();
    if (!light)
    {
        m_lineEdit_color_r->setText(QString::number(0));
        m_lineEdit_color_g->setText(QString::number(0));
        m_lineEdit_color_b->setText(QString::number(0));

        m_lineEdit_intensity->setText(QString::number(0));

        m_lineEdit_direction_x->setText(QString::number(0));
        m_lineEdit_direction_y->setText(QString::number(0));
        m_lineEdit_direction_z->setText(QString::number(0));
    }
    else
    {
        auto color  = light->color;
        auto factor = light->factor;
        auto dir    = light->direction;

        m_lineEdit_color_r->setText(QString::number(color.x));
        m_lineEdit_color_g->setText(QString::number(color.y));
        m_lineEdit_color_b->setText(QString::number(color.z));

        m_lineEdit_intensity->setText(QString::number(factor));

        m_lineEdit_direction_x->setText(QString::number(dir.x));
        m_lineEdit_direction_y->setText(QString::number(dir.y));
        m_lineEdit_direction_z->setText(QString::number(dir.z));
    }
}

void LightWidget::_show_hdr_ui()
{
    m_label_texture->show();
    m_lineEdit_texture->show();

    auto light = m_scene->get_hdr_light();
    if (!light)
    {
        m_lineEdit_color_r->setText(QString::number(0));
        m_lineEdit_color_g->setText(QString::number(0));
        m_lineEdit_color_b->setText(QString::number(0));

        m_lineEdit_intensity->setText(QString::number(0));

        m_lineEdit_texture->setText(QString::number(-1));
    }
    else
    {
        auto color  = light->color;
        auto factor = light->factor;
        auto id     = light->id;

        m_lineEdit_color_r->setText(QString::number(color.x));
        m_lineEdit_color_g->setText(QString::number(color.y));
        m_lineEdit_color_b->setText(QString::number(color.z));

        m_lineEdit_intensity->setText(QString::number(factor));

        m_lineEdit_texture->setText(QString::number(id.idx));
    }
}

void LightWidget::_show_point_ui()
{
    m_label_position->show();
    m_lineEdit_position_x->show();
    m_lineEdit_position_y->show();
    m_lineEdit_position_z->show();

    auto& light  = std::get<acre::PointLight>(m_light->light);
    auto  color  = light.color;
    auto  factor = light.factor;
    auto  pos    = light.position;

    m_lineEdit_color_r->setText(QString::number(color.x));
    m_lineEdit_color_g->setText(QString::number(color.y));
    m_lineEdit_color_b->setText(QString::number(color.z));

    m_lineEdit_intensity->setText(QString::number(factor));

    m_lineEdit_position_x->setText(QString::number(pos.x));
    m_lineEdit_position_y->setText(QString::number(pos.y));
    m_lineEdit_position_z->setText(QString::number(pos.z));
}

void LightWidget::_hide_direction_ui()
{
    m_label_direction->hide();
    m_lineEdit_direction_x->hide();
    m_lineEdit_direction_y->hide();
    m_lineEdit_direction_z->hide();
}

void LightWidget::_hide_hdr_ui()
{
    m_label_texture->hide();
    m_lineEdit_texture->hide();
}

void LightWidget::_hide_point_ui()
{
    m_label_position->hide();
    m_lineEdit_position_x->hide();
    m_lineEdit_position_y->hide();
    m_lineEdit_position_z->hide();
}

void LightWidget::_on_update_color()
{
    auto r = m_lineEdit_color_r->text();
    auto g = m_lineEdit_color_g->text();
    auto b = m_lineEdit_color_b->text();

    auto rgb = acre::math::float3(r.toFloat(), g.toFloat(), b.toFloat());

    if (m_useSun)
    {
        auto light = m_scene->get_sun_light();
        if (!light) return;
        light->color = rgb;
    }
    else if (m_useHDR)
    {
        auto light = m_scene->get_hdr_light();
        if (!light) return;
        light->color = rgb;
    }
    else
    {
        if (!m_light) return;
        switch (m_light->type)
        {
            case acre::LightType::Point:
            {
                auto& light = std::get<acre::PointLight>(m_light->light);
                light.color = rgb;
                m_scene->update_light(m_lightR);
            }
        }
    }

    m_renderframe_func();
}

void LightWidget::_on_update_intensity()
{
    auto value = m_lineEdit_intensity->text();

    if (m_useSun)
    {
        auto light = m_scene->get_sun_light();
        if (!light) return;
        light->factor = value.toFloat();
    }
    else if (m_useHDR)
    {
        auto light = m_scene->get_hdr_light();
        if (!light) return;
        light->factor = value.toFloat();
    }
    else
    {
        if (!m_light) return;
        switch (m_light->type)
        {
            case acre::LightType::Point:
            {
                auto& light  = std::get<acre::PointLight>(m_light->light);
                light.factor = value.toFloat();
                m_scene->update_light(m_lightR);
            }
        }
    }

    m_renderframe_func();
}

void LightWidget::_on_update_direction()
{
    auto light = m_scene->get_sun_light();
    if (!light) return;

    auto x = m_lineEdit_direction_x->text();
    auto y = m_lineEdit_direction_y->text();
    auto z = m_lineEdit_direction_z->text();

    light->direction = acre::math::float3(x.toFloat(), y.toFloat(), z.toFloat());
    m_renderframe_func();
}

void LightWidget::_on_update_texture()
{
    auto light = m_scene->get_hdr_light();
    if (!light) return;

    auto value    = m_lineEdit_texture->text();
    light->id.idx = value.toInt();
    m_renderframe_func();
}

void LightWidget::_on_update_position()
{
    if (!m_light) return;

    auto x = m_lineEdit_position_x->text();
    auto y = m_lineEdit_position_y->text();
    auto z = m_lineEdit_position_z->text();

    switch (m_light->type)
    {
        case acre::LightType::Point:
        {
            auto& light    = std::get<acre::PointLight>(m_light->light);
            light.position = acre::math::float3(x.toFloat(), y.toFloat(), z.toFloat());
        }
    }

    m_scene->update_light(m_lightR);
    m_renderframe_func();
}
