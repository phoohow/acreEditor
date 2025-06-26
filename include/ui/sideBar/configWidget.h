#pragma once

#include <QWidget>

class BaseScene;
class ConfigWidget : public QWidget
{
    BaseScene* m_scene;

public:
    explicit ConfigWidget(BaseScene* scene, QWidget* parent = nullptr);

    ~ConfigWidget();

private:
    void setupUi();
};
