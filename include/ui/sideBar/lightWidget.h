#pragma once

#include <acre/render/scene/light.h>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>

#include <functional>

class BaseScene;
class LightWidget : public QWidget
{
    enum LightType
    {
        Sun,
        HDR,
        Point,
    };

    BaseScene* m_scene;

    std::function<void()> m_flushFrame;

    acre::LightPtr m_light   = nullptr;
    acre::LightID  m_lightID = -1;
    LightType     m_type;
    bool          m_useSun = false;
    bool          m_useHDR = false;

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
    explicit LightWidget(BaseScene* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func) { m_flushFrame = func; }

    void setLight(acre::LightID id);

    void enableSun();
    void disableSun();

    void enableHDR();
    void disableHDR();

    void updateProperties();

    void updateSunProperties();

    void updateHDRProperties();

private:
    void initUI();
    void initDirectionUI();
    void initImageUI();
    void initPointUI();
    void showSunUI();
    void showHDRUI();
    void showPointUI();
    void hideDirectionUI();
    void hideHDRUI();
    void hidePointUI();

    void onUpdateType(int index);

    void onUpdateColor();
    void onUpdateIntensity();
    void onUpdateDirection();
    void onUpdateTexture();
    void onUpdatePosition();
};
