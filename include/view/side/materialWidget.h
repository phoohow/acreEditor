#pragma once

#include <acre/render/scene/material.h>

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>

#include <functional>

class SceneMgr;
namespace acre
{
struct Resource;
}
class MaterialWidget : public QWidget
{
    SceneMgr* m_scene;

    std::function<void()> m_renderframe_func;

    acre::Resource* m_materialR = nullptr;
    acre::Material* m_material  = nullptr;

    QVBoxLayout* m_layout;

    QHBoxLayout* m_layout_type;
    QLabel*      m_label_type;
    QComboBox*   m_comboBox_type;

    QLabel*    m_label_baseColor;
    QLineEdit* m_lineEdit_baseColor_r;
    QLineEdit* m_lineEdit_baseColor_g;
    QLineEdit* m_lineEdit_baseColor_b;

    QLabel*    m_label_alpha;
    QLineEdit* m_lineEdit_alpha;

    QLabel*    m_label_roughness;
    QLineEdit* m_lineEdit_roughness;

    QLabel*    m_label_metal;
    QLineEdit* m_lineEdit_metal;

    QLabel*    m_label_emissive;
    QLineEdit* m_lineEdit_emissive_r;
    QLineEdit* m_lineEdit_emissive_g;
    QLineEdit* m_lineEdit_emissive_b;

    QLabel*    m_label_basecolor_map;
    QLineEdit* m_line_edit_basecolor_map;

    QLabel*    m_label_metal_rough_map;
    QLineEdit* m_line_edit_metal_rough_map;

    QLabel*    m_label_emissive_map;
    QLineEdit* m_line_edit_emissive_map;

    QLabel*    m_label_normal_map;
    QLineEdit* m_line_edit_normal_map;

public:
    explicit MaterialWidget(SceneMgr* scene, QWidget* parent = nullptr);

    ~MaterialWidget();

    void set_renderframe_callback(std::function<void()> func) { m_renderframe_func = func; }

    void set_material(uint32_t uuid);

    void update_properties();

private:
    void _init_ui();
    void _update_type();

    void _on_update_basecolor();
    void _on_update_alpha();
    void _on_update_roughness();
    void _on_update_metallic();
    void _on_update_emissive();
    void _on_update_basecolor_map();
    void _on_update_normal_map();
    void _on_update_emissivel_map();
    void _on_update_metal_rough_map();
};
