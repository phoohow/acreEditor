#pragma once

#include <acre/render/scene/light.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>

#include <functional>

class SceneMgr;
namespace acre
{
struct Resource;
}
class LightWidget : public QWidget
{
    enum LightType
    {
        Sun,
        HDR,
        Point,
    };

    SceneMgr* m_scene;

    std::function<void()> m_renderframe_func;

    acre::Resource* m_lightR = nullptr;
    acre::Light*    m_light  = nullptr;
    LightType       m_type;
    bool            m_useSun = false;
    bool            m_useHDR = false;

    QVBoxLayout* m_layout;

    QHBoxLayout* m_layout_type;
    QLabel*      m_label_type;
    QComboBox*   m_comboBox_type;

    QHBoxLayout* m_layout_direction;
    QLabel*      m_label_direction;
    QLineEdit*   m_lineEdit_direction_x;
    QLineEdit*   m_lineEdit_direction_y;
    QLineEdit*   m_lineEdit_direction_z;

    QHBoxLayout* m_layout_intensity;
    QLabel*      m_label_intensity;
    QLineEdit*   m_lineEdit_intensity;

    QHBoxLayout* m_layout_texture;
    QLabel*      m_label_texture;
    QLineEdit*   m_lineEdit_texture;

    QHBoxLayout* m_layout_color;
    QLabel*      m_label_color;
    QLineEdit*   m_lineEdit_color_r;
    QLineEdit*   m_lineEdit_color_g;
    QLineEdit*   m_lineEdit_color_b;

    QHBoxLayout* m_layout_position;
    QLabel*      m_label_position;
    QLineEdit*   m_lineEdit_position_x;
    QLineEdit*   m_lineEdit_position_y;
    QLineEdit*   m_lineEdit_position_z;

public:
    explicit LightWidget(SceneMgr* scene, QWidget* parent = nullptr);

    void set_renderframe_callback(std::function<void()> func) { m_renderframe_func = func; }

    void set_light(uint32_t uuid);

    void enable_sun();
    void disable_sun();

    void enable_hdr();
    void disable_hdr();

    void update_properties();

    void update_sun_properties();

    void update_hdr_properties();

private:
    void _init_ui();
    void _init_direction_ui();
    void _init_image_ui();
    void _init_point_ui();
    void _show_sun_ui();
    void _show_hdr_ui();
    void _show_point_ui();
    void _hide_direction_ui();
    void _hide_hdr_ui();
    void _hide_point_ui();

    void _on_update_type(int index);

    void _on_update_color();
    void _on_update_intensity();
    void _on_update_direction();
    void _on_update_texture();
    void _on_update_position();
};
