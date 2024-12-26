#pragma once

#include <acre/render/scene/material.h>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QComboBox>

#include <functional>

class BaseScene;
class MaterialWidget : public QWidget
{
    BaseScene* m_scene;

    std::function<void()> m_flushFrame;

    acre::MaterialID  m_materialID = -1;
    acre::MaterialPtr m_material;

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

    QLabel*    m_label_baseColorMap;
    QLineEdit* m_lineEdit_baseColorMap;

    QLabel*    m_label_metalRoughMap;
    QLineEdit* m_lineEdit_metalRoughMap;

    QLabel*    m_label_emissiveMap;
    QLineEdit* m_lineEdit_emissiveMap;

    QLabel*    m_label_normalMap;
    QLineEdit* m_lineEdit_normalMap;

public:
    explicit MaterialWidget(BaseScene* scene, QWidget* parent = nullptr);

    ~MaterialWidget();

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

    void setMaterial(acre::MaterialID id);

    void updateProperties();

private:
    void initUI();
    void updateType();

    void onUpdateBaseColor();
    void onUpdateAlpha();
    void onUpdateRoughness();
    void onUpdateMetallic();
    void onUpdateEmissive();
    void onUpdateBaseColorMap();
    void onUpdateNormalMap();
    void onUpdateEmissiveMap();
    void onUpdateMetalRoughMap();
};
