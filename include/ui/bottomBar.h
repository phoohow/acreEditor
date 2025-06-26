#pragma once
#include <ui/bottomBar/cmdWidget.h>
#include <ui/bottomBar/infoWidget.h>

#include <functional>

class BaseScene;
class BottomBar : public QWidget
{
    BaseScene* m_scene = nullptr;

    std::function<void()> m_flushFrame;

    QVBoxLayout* m_mainLayout;
    InfoWidget*  m_infoWidget;
    QVBoxLayout* m_infoLayout;
    CmdWidget*   m_cmdWidget;
    QVBoxLayout* m_cmdLayout;

public:
    explicit BottomBar(BaseScene* scene, QWidget* parent = nullptr);

    void setFlushFrameCallBack(std::function<void()> func);

    void flushState();
};
